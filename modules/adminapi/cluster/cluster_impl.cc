/*
 * Copyright (c) 2016, 2024, Oracle and/or its affiliates.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0,
 * as published by the Free Software Foundation.
 *
 * This program is designed to work with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have either included with
 * the program or referenced in the documentation.
 *
 * This program is distributed in the hope that it will be useful,  but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "modules/adminapi/cluster/cluster_impl.h"

#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "modules/adminapi/cluster/check_instance_state.h"
#include "modules/adminapi/cluster/describe.h"
#include "modules/adminapi/cluster/dissolve.h"
#include "modules/adminapi/cluster/options.h"
#include "modules/adminapi/cluster/rescan.h"
#include "modules/adminapi/cluster/reset_recovery_accounts_password.h"
#include "modules/adminapi/cluster/set_instance_option.h"
#include "modules/adminapi/cluster/set_option.h"
#include "modules/adminapi/cluster/set_primary_instance.h"
#include "modules/adminapi/cluster/status.h"
#include "modules/adminapi/cluster/switch_to_multi_primary_mode.h"
#include "modules/adminapi/cluster/switch_to_single_primary_mode.h"
#include "modules/adminapi/cluster_set/cluster_set_impl.h"
#include "modules/adminapi/common/accounts.h"
#include "modules/adminapi/common/async_topology.h"
#include "modules/adminapi/common/cluster_topology_executor.h"
#include "modules/adminapi/common/cluster_types.h"
#include "modules/adminapi/common/common.h"
#include "modules/adminapi/common/dba_errors.h"
#include "modules/adminapi/common/errors.h"
#include "modules/adminapi/common/group_replication_options.h"
#include "modules/adminapi/common/instance_validations.h"
#include "modules/adminapi/common/metadata_storage.h"
#include "modules/adminapi/common/preconditions.h"
#include "modules/adminapi/common/provision.h"
#include "modules/adminapi/common/router.h"
#include "modules/adminapi/common/server_features.h"
#include "modules/adminapi/common/sql.h"
#include "modules/adminapi/common/validations.h"
#include "modules/adminapi/dba_utils.h"
#include "modules/adminapi/mod_dba_cluster.h"
#include "modules/mysqlxtest_utils.h"
#include "mysqlshdk/include/shellcore/console.h"
#include "mysqlshdk/libs/config/config_server_handler.h"
#include "mysqlshdk/libs/mysql/async_replication.h"
#include "mysqlshdk/libs/mysql/clone.h"
#include "mysqlshdk/libs/mysql/group_replication.h"
#include "mysqlshdk/libs/mysql/replication.h"
#include "mysqlshdk/libs/mysql/undo.h"
#include "mysqlshdk/libs/mysql/utils.h"
#include "mysqlshdk/libs/textui/textui.h"
#include "mysqlshdk/libs/utils/utils_net.h"
#include "scripting/types.h"
#include "shellcore/utils_help.h"
#include "utils/debug.h"
#include "utils/utils_general.h"

namespace mysqlsh {
namespace dba {

namespace {
// Constants with the names used to lock the cluster
constexpr char k_lock_ns[] = "AdminAPI_cluster";
constexpr char k_lock_name[] = "AdminAPI_lock";

template <typename T>
struct Option_info {
  bool found_non_default = false;
  bool found_not_supported = false;
  T non_default_value = T{};
};

void check_gr_empty_local_address_exception(
    const shcore::Exception &exp, const mysqlshdk::mysql::IInstance &instance) {
  if ((exp.code() != SHERR_DBA_EMPTY_LOCAL_ADDRESS)) return;

  mysqlsh::current_console()->print_error(shcore::str_format(
      "Unable to read Group Replication local address setting for instance "
      "'%s', probably due to connectivity issues. Please retry the operation.",
      instance.get_canonical_address().c_str()));
}

int64_t extract_server_id(std::string_view account_user) {
  auto pos = account_user.find_last_of('_');
  if (pos == std::string_view::npos) return -1;

  account_user = account_user.substr(pos + 1);

  return shcore::lexical_cast<int64_t>(account_user, -1);
}
}  // namespace

Cluster_impl::Cluster_impl(
    const std::shared_ptr<Cluster_set_impl> &cluster_set,
    const Cluster_metadata &metadata,
    const std::shared_ptr<Instance> &group_server,
    const std::shared_ptr<MetadataStorage> &metadata_storage,
    Cluster_availability availability)
    : Cluster_impl(metadata, group_server, metadata_storage, availability) {
  m_cluster_set = cluster_set;
  if (auto primary = cluster_set->get_primary_master(); primary)
    m_primary_master = primary;
}

Cluster_impl::Cluster_impl(
    const Cluster_metadata &metadata,
    const std::shared_ptr<Instance> &group_server,
    const std::shared_ptr<MetadataStorage> &metadata_storage,
    Cluster_availability availability)
    : Base_cluster_impl(metadata.cluster_name, group_server, metadata_storage),
      m_topology_type(metadata.cluster_topology_type),
      m_availability(availability),
      m_cs_md() {
  m_id = metadata.cluster_id;
  m_description = metadata.description;
  m_group_name = metadata.group_name;
  m_primary_master = group_server;

  observe_notification(kNotifyClusterSetPrimaryChanged);
}

Cluster_impl::Cluster_impl(
    const std::string &cluster_name, const std::string &group_name,
    const std::shared_ptr<Instance> &group_server,
    const std::shared_ptr<Instance> &primary_master,
    const std::shared_ptr<MetadataStorage> &metadata_storage,
    mysqlshdk::gr::Topology_mode topology_type)
    : Base_cluster_impl(cluster_name, group_server, metadata_storage),
      m_group_name(group_name),
      m_topology_type(topology_type) {
  // cluster is being created

  m_primary_master = primary_master;

  assert(topology_type == mysqlshdk::gr::Topology_mode::SINGLE_PRIMARY ||
         topology_type == mysqlshdk::gr::Topology_mode::MULTI_PRIMARY);

  observe_notification(kNotifyClusterSetPrimaryChanged);
}

Cluster_impl::~Cluster_impl() = default;

void Cluster_impl::sanity_check() const {
  if (m_availability == Cluster_availability::ONLINE)
    verify_topology_type_change();
}

std::string Cluster_impl::get_replication_user_host() const {
  auto md = get_metadata_storage();

  shcore::Value allowed_host;
  if (md->query_cluster_attribute(get_id(),
                                  k_cluster_attribute_replication_allowed_host,
                                  &allowed_host) &&
      allowed_host.type == shcore::String) {
    if (auto host = allowed_host.as_string(); !host.empty()) return host;
  }

  return "%";
}

void Cluster_impl::find_real_cluster_set_primary(Cluster_set_impl *cs) const {
  assert(cs);

  for (;;) {
    if (!m_primary_master || !cs->get_primary_cluster()) {
      cs->connect_primary();
    }

    if (!cs->reconnect_target_if_invalidated(false)) {
      break;
    }
  }
}

/*
 * Verify if the topology type changed and issue an error if needed
 */
void Cluster_impl::verify_topology_type_change() const {
  // TODO(alfredo) - this should be replaced by a clusterErrors node in
  // cluster.status(), along with other cluster level diag msgs

  // Get the primary UUID value to determine GR mode:
  // UUID (not empty) -> single-primary or "" (empty) -> multi-primary

  std::string gr_primary_uuid =
      mysqlshdk::gr::get_group_primary_uuid(*get_cluster_server(), nullptr);

  // Check if the topology type matches the real settings used by the
  // cluster instance, otherwise an error is issued.
  // NOTE: The GR primary mode is guaranteed (by GR) to be the same for all
  // instance of the same group.
  if (!gr_primary_uuid.empty() &&
      get_cluster_topology_type() ==
          mysqlshdk::gr::Topology_mode::MULTI_PRIMARY) {
    throw shcore::Exception::runtime_error(
        "The InnoDB Cluster topology type (Multi-Primary) does not match the "
        "current Group Replication configuration (Single-Primary). Please "
        "use <cluster>.rescan() or change the Group Replication "
        "configuration accordingly.");
  } else if (gr_primary_uuid.empty() &&
             get_cluster_topology_type() ==
                 mysqlshdk::gr::Topology_mode::SINGLE_PRIMARY) {
    throw shcore::Exception::runtime_error(
        "The InnoDB Cluster topology type (Single-Primary) does not match the "
        "current Group Replication configuration (Multi-Primary). Please "
        "use <cluster>.rescan() or change the Group Replication "
        "configuration accordingly.");
  }
}

void Cluster_impl::validate_rejoin_gtid_consistency(
    const mysqlshdk::mysql::IInstance &target_instance) const {
  auto console = mysqlsh::current_console();
  std::string errant_gtid_set;

  // Get the gtid state in regards to the cluster_session
  mysqlshdk::mysql::Replica_gtid_state state =
      mysqlshdk::mysql::check_replica_gtid_state(
          *get_cluster_server(), target_instance, nullptr, &errant_gtid_set);

  if (state == mysqlshdk::mysql::Replica_gtid_state::NEW) {
    console->print_info();
    console->print_error(
        "The target instance '" + target_instance.descr() +
        "' has an empty GTID set so it cannot be safely rejoined to the "
        "cluster. Please remove it and add it back to the cluster.");
    console->print_info();

    throw shcore::Exception::runtime_error("The instance '" +
                                           target_instance.descr() +
                                           "' has an empty GTID set.");
  } else if (state == mysqlshdk::mysql::Replica_gtid_state::IRRECOVERABLE) {
    console->print_info();
    console->print_error("A GTID set check of the MySQL instance at '" +
                         target_instance.descr() +
                         "' determined that it is missing transactions that "
                         "were purged from all cluster members.");
    console->print_info();
    throw shcore::Exception::runtime_error(
        "The instance '" + target_instance.descr() +
        "' is missing transactions that "
        "were purged from all cluster members.");
  } else if (state == mysqlshdk::mysql::Replica_gtid_state::DIVERGED) {
    console->print_info();
    console->print_error("A GTID set check of the MySQL instance at '" +
                         target_instance.descr() +
                         "' determined that it contains transactions that "
                         "do not originate from the cluster, which must be "
                         "discarded before it can join the cluster.");

    console->print_info();
    console->print_info(target_instance.descr() +
                        " has the following errant GTIDs that do not exist "
                        "in the cluster:\n" +
                        errant_gtid_set);
    console->print_info();

    console->print_info(
        "Having extra GTID events is not expected, and it is "
        "recommended to investigate this further and ensure that the data "
        "can be removed prior to rejoining the instance to the cluster.");

    if (supports_mysql_clone(target_instance.get_version())) {
      console->print_info();
      console->print_info(
          "Discarding these extra GTID events can either be done manually "
          "or by completely overwriting the state of " +
          target_instance.descr() +
          " with a physical snapshot from an existing cluster member. To "
          "achieve this remove the instance from the cluster and add it "
          "back using <Cluster>.<<<addInstance>>>() and setting the "
          "'recoveryMethod' option to 'clone'.");
    }

    console->print_info();

    throw shcore::Exception::runtime_error(
        "The instance '" + target_instance.descr() +
        "' contains errant transactions that did not originate from the "
        "cluster.");
  }
}

void Cluster_impl::adopt_from_gr() {
  shcore::Value ret_val;
  auto console = mysqlsh::current_console();

  auto newly_discovered_instances_list(get_newly_discovered_instances(
      *get_cluster_server(), get_metadata_storage(), get_id()));

  // Add all instances to the cluster metadata
  for (NewInstanceInfo &instance : newly_discovered_instances_list) {
    mysqlshdk::db::Connection_options newly_discovered_instance;

    newly_discovered_instance.set_host(instance.host);
    newly_discovered_instance.set_port(instance.port);

    log_info("Adopting member %s:%d from existing group", instance.host.c_str(),
             instance.port);
    console->print_info("Adding Instance '" + instance.host + ":" +
                        std::to_string(instance.port) + "'...");

    auto md_instance = get_metadata_storage()->get_md_server();

    auto session_data = md_instance->get_connection_options();

    newly_discovered_instance.set_login_options_from(session_data);

    add_metadata_for_instance(newly_discovered_instance);

    // Store the communicationStack in the Metadata as a Cluster capability
    // TODO(miguel): build and add the list of allowed operations
    get_metadata_storage()->update_cluster_capability(
        get_id(), kCommunicationStack,
        get_communication_stack(*get_cluster_server()),
        std::set<std::string>());
  }
}

/** Iterates through all the cluster members in a given state calling the given
 * function on each of then.
 * @param states Vector of strings with the states of members on which the
 * functor will be called.
 * @param cnx_opt Connection options to be used to connect to the cluster
 * members
 * @param ignore_instances_vector Vector with addresses of instances to be
 * ignored even if their state is specified in the states vector.
 * @param functor Function that is called on each member of the cluster whose
 * state is specified in the states vector.
 * @param condition Optional condition function. If provided, the result will be
 * evaluated and the functor will only be called if the condition returns true.
 * @param ignore_network_conn_errors Optional flag to indicate whether
 * connection errors should be treated as errors and stop execution in remaining
 * instances, by default is set to true.
 */
void Cluster_impl::execute_in_members(
    const std::vector<mysqlshdk::gr::Member_state> &states,
    const mysqlshdk::db::Connection_options &cnx_opt,
    const std::vector<std::string> &ignore_instances_vector,
    const std::function<bool(const std::shared_ptr<Instance> &instance,
                             const mysqlshdk::gr::Member &gr_member)> &functor,
    const std::function<
        bool(const Instance_md_and_gr_member &instance_with_state)> &condition,
    bool ignore_network_conn_errors) const {
  std::shared_ptr<Instance> instance_session;
  // Note (nelson): should we handle the super_read_only behavior here or should
  // it be the responsibility of the functor?
  auto instance_definitions = get_instances_with_state();

  for (auto &instance_def : instance_definitions) {
    // If the condition functor is given and the condition is not met then
    // continues to the next member
    if (condition && !condition(instance_def)) continue;

    std::string instance_address = instance_def.first.endpoint;

    // if instance is on the list of instances to be ignored, skip it
    if (std::find_if(ignore_instances_vector.begin(),
                     ignore_instances_vector.end(),
                     mysqlshdk::utils::Endpoint_predicate{instance_address}) !=
        ignore_instances_vector.end()) {
      continue;
    }
    // if state list is given but it doesn't match, skip too
    if (!states.empty() &&
        std::find(states.begin(), states.end(), instance_def.second.state) ==
            states.end()) {
      continue;
    }

    auto target_coptions =
        shcore::get_connection_options(instance_address, false);

    target_coptions.set_login_options_from(cnx_opt);
    try {
      log_debug(
          "Opening a new session to instance '%s' while iterating "
          "cluster members",
          instance_address.c_str());
      instance_session = Instance::connect(
          target_coptions, current_shell_options()->get().wizards);
    } catch (const shcore::Error &e) {
      if (ignore_network_conn_errors && e.code() == CR_CONN_HOST_ERROR) {
        log_error("Could not open connection to '%s': %s, but ignoring it.",
                  instance_address.c_str(), e.what());
        continue;
      } else {
        log_error("Could not open connection to '%s': %s",
                  instance_address.c_str(), e.what());
        throw;
      }
    } catch (const std::exception &e) {
      log_error("Could not open connection to '%s': %s",
                instance_address.c_str(), e.what());
      throw;
    }
    bool continue_loop = functor(instance_session, instance_def.second);
    if (!continue_loop) {
      log_debug("Cluster iteration stopped because functor returned false.");
      break;
    }
  }
}

void Cluster_impl::execute_in_members(
    const std::function<bool(const std::shared_ptr<Instance> &instance,
                             const Instance_md_and_gr_member &info)>
        &on_connect,
    const std::function<bool(const shcore::Error &error,
                             const Instance_md_and_gr_member &info)>
        &on_connect_error) const {
  auto instance_definitions = get_instances_with_state(true);
  auto ipool = current_ipool();

  for (const auto &i : instance_definitions) {
    Scoped_instance instance_session;

    try {
      instance_session =
          Scoped_instance(ipool->connect_unchecked_endpoint(i.first.endpoint));
    } catch (const shcore::Error &e) {
      if (on_connect_error) {
        if (!on_connect_error(e, i)) break;
        continue;
      } else {
        throw;
      }
    }
    if (!on_connect(instance_session, i)) break;
  }
}

mysqlshdk::db::Connection_options Cluster_impl::pick_seed_instance() const {
  bool single_primary;
  std::string primary_uuid = mysqlshdk::gr::get_group_primary_uuid(
      *get_cluster_server(), &single_primary);
  if (single_primary) {
    if (!primary_uuid.empty()) {
      Instance_metadata info =
          get_metadata_storage()->get_instance_by_uuid(primary_uuid);

      mysqlshdk::db::Connection_options coptions(info.endpoint);
      mysqlshdk::db::Connection_options group_session_target(
          get_cluster_server()->get_connection_options());

      coptions.set_login_options_from(group_session_target);

      return coptions;
    }
    throw shcore::Exception::runtime_error(
        "Unable to determine a suitable peer instance to join the group");
  } else {
    // instance we're connected to should be OK if we're multi-master
    return get_cluster_server()->get_connection_options();
  }
}

void Cluster_impl::validate_variable_compatibility(
    const mysqlshdk::mysql::IInstance &target_instance,
    const std::string &var_name) const {
  auto cluster_inst = get_cluster_server();
  auto instance_val = target_instance.get_sysvar_string(var_name).value_or("");
  auto cluster_val = cluster_inst->get_sysvar_string(var_name).value_or("");
  std::string instance_address =
      target_instance.get_connection_options().as_uri(
          mysqlshdk::db::uri::formats::only_transport());
  log_info(
      "Validating if '%s' variable has the same value on target instance '%s' "
      "as it does on the cluster.",
      var_name.c_str(), instance_address.c_str());

  // If values are different between cluster and target instance throw an
  // exception.
  if (instance_val == cluster_val) return;

  auto console = mysqlsh::current_console();
  console->print_error(shcore::str_format(
      "Cannot join instance '%s' to cluster: incompatible '%s' value.",
      instance_address.c_str(), var_name.c_str()));
  throw shcore::Exception::runtime_error(
      shcore::str_format("The '%s' value '%s' of the instance '%s' is "
                         "different from the value of the cluster '%s'.",
                         var_name.c_str(), instance_val.c_str(),
                         instance_address.c_str(), cluster_val.c_str()));
}

/**
 * Returns group seeds for all members of the cluster
 *
 * @return (server_uuid, endpoint) map of all group seeds of the cluster
 */
std::map<std::string, std::string> Cluster_impl::get_cluster_group_seeds()
    const {
  std::map<std::string, std::string> group_seeds;

  auto instances = get_metadata_storage()->get_all_instances(get_id());
  for (const auto &i : instances) {
    assert(!i.grendpoint.empty());
    assert(!i.uuid.empty());
    if (i.grendpoint.empty() || i.uuid.empty())
      log_error("Metadata for instance %s is invalid (grendpoint=%s, uuid=%s)",
                i.endpoint.c_str(), i.grendpoint.c_str(), i.uuid.c_str());

    group_seeds[i.uuid] = i.grendpoint;
  }

  return group_seeds;
}

std::vector<Instance_metadata> Cluster_impl::get_instances(
    const std::vector<mysqlshdk::gr::Member_state> &states) const {
  std::vector<Instance_metadata> all_instances =
      get_metadata_storage()->get_all_instances(get_id());

  if (states.empty()) return all_instances;

  std::vector<mysqlshdk::gr::Member> members =
      mysqlshdk::gr::get_members(*get_cluster_server());

  std::vector<Instance_metadata> res;
  res.reserve(all_instances.size());

  for (const auto &i : all_instances) {
    auto m = std::find_if(members.begin(), members.end(),
                          [&i](const mysqlshdk::gr::Member &member) {
                            return member.uuid == i.uuid;
                          });
    if (m != members.end() &&
        std::find(states.begin(), states.end(), m->state) != states.end()) {
      res.push_back(i);
    }
  }
  return res;
}

void Cluster_impl::ensure_metadata_has_server_id(
    const mysqlshdk::mysql::IInstance &target_instance) const {
  execute_in_members(
      {}, target_instance.get_connection_options(), {},
      [this](const std::shared_ptr<Instance> &instance,
             const mysqlshdk::gr::Member &gr_member) {
        get_metadata_storage()->update_instance_attribute(
            gr_member.uuid, k_instance_attribute_server_id,
            shcore::Value(instance->get_server_id()));

        return true;
      },
      [](const Instance_md_and_gr_member &md) {
        return md.first.server_id == 0 &&
               (md.second.state == mysqlshdk::gr::Member_state::ONLINE ||
                md.second.state == mysqlshdk::gr::Member_state::RECOVERING);
      });
}

void Cluster_impl::ensure_metadata_has_recovery_accounts() {
  auto endpoints =
      get_metadata_storage()->get_instances_with_recovery_accounts(get_id());

  if (endpoints.empty()) return;

  log_info("Fixing instances missing the replication recovery account...");

  auto console = mysqlsh::current_console();

  execute_in_members(
      {}, get_cluster_server()->get_connection_options(), {},
      [this, &console, &endpoints](const std::shared_ptr<Instance> &instance,
                                   const mysqlshdk::gr::Member &gr_member) {
        std::string recovery_user = mysqlshdk::mysql::get_replication_user(
            *instance, mysqlshdk::gr::k_gr_recovery_channel);

        log_debug("Fixing recovering account '%s' in instance '%s'",
                  recovery_user.c_str(), instance->descr().c_str());

        bool recovery_is_valid = false;
        if (!recovery_user.empty()) {
          recovery_is_valid =
              shcore::str_beginswith(
                  recovery_user,
                  mysqlshdk::gr::k_group_recovery_old_user_prefix) ||
              shcore::str_beginswith(
                  recovery_user, mysqlshdk::gr::k_group_recovery_user_prefix);
        }

        if (!recovery_user.empty()) {
          if (!recovery_is_valid) {
            console->print_error(shcore::str_format(
                "Unsupported recovery account '%s' has been found for instance "
                "'%s'. Operations such as "
                "<Cluster>.<<<resetRecoveryAccountsPassword>>>() and "
                "<Cluster>.<<<addInstance>>>() may fail. Please remove and add "
                "the instance back to the Cluster to ensure a supported "
                "recovery account is used.",
                recovery_user.c_str(), instance->descr().c_str()));
            return true;
          }

          auto it = endpoints.find(instance->get_uuid());
          if ((it != endpoints.end()) && (it->second != recovery_user)) {
            get_metadata_storage()->update_instance_repl_account(
                gr_member.uuid, Cluster_type::GROUP_REPLICATION, recovery_user,
                get_replication_user_host());
          }

        } else {
          throw std::logic_error(
              "Recovery user account not found for server address: " +
              instance->descr() + " with UUID " + instance->get_uuid());
        }

        return true;
      },
      [](const Instance_md_and_gr_member &md) {
        return (md.second.state == mysqlshdk::gr::Member_state::ONLINE ||
                md.second.state == mysqlshdk::gr::Member_state::RECOVERING);
      });
}

std::vector<Instance_metadata> Cluster_impl::get_active_instances(
    bool online_only) const {
  if (online_only)
    return get_instances({mysqlshdk::gr::Member_state::ONLINE});
  else
    return get_instances({mysqlshdk::gr::Member_state::ONLINE,
                          mysqlshdk::gr::Member_state::RECOVERING});
}

std::shared_ptr<mysqlsh::dba::Instance> Cluster_impl::get_online_instance(
    const std::string &exclude_uuid) const {
  std::vector<Instance_metadata> instances(get_active_instances());

  // Get the cluster connection credentials to use to connect to instances.
  mysqlshdk::db::Connection_options cluster_cnx_opts =
      get_cluster_server()->get_connection_options();

  for (const auto &instance : instances) {
    // Skip instance with the provided UUID exception (if specified).
    if (exclude_uuid.empty() || instance.uuid != exclude_uuid) {
      try {
        // Use the cluster connection credentials.
        mysqlshdk::db::Connection_options coptions(instance.endpoint);
        coptions.set_login_options_from(cluster_cnx_opts);

        log_info("Opening session to the member of InnoDB cluster at %s...",
                 coptions.as_uri().c_str());

        // Return the first valid (reachable) instance.
        return Instance::connect(coptions);
      } catch (const std::exception &e) {
        log_debug(
            "Unable to establish a session to the cluster member '%s': %s",
            instance.endpoint.c_str(), e.what());
      }
    }
  }

  // No reachable online instance was found.
  return nullptr;
}

/**
 * Check the instance server UUID of the specified instance.
 *
 * The server UUID must be unique for all instances in a cluster. This function
 * checks if the server_uuid of the target instance is unique among all
 * members of the cluster.
 *
 * @param instance_session Session to the target instance to check its server
 *                         UUID.
 */
void Cluster_impl::validate_server_uuid(
    const mysqlshdk::mysql::IInstance &instance) const {
  // Get the server_uuid of the target instance.
  std::string server_uuid = *instance.get_sysvar_string(
      "server_uuid", mysqlshdk::mysql::Var_qualifier::GLOBAL);

  // Get connection option for the metadata.

  std::shared_ptr<Instance> cluster_instance = get_cluster_server();
  Connection_options cluster_cnx_opt =
      cluster_instance->get_connection_options();

  // Get list of instances in the metadata
  std::vector<Instance_metadata> metadata_instances = get_instances();

  // Get and compare the server UUID of all instances with the one of
  // the target instance.
  for (Instance_metadata &instance_def : metadata_instances) {
    if (server_uuid == instance_def.uuid) {
      // Raise an error if the server uuid is the same of a cluster member.
      throw shcore::Exception::runtime_error(
          "Cannot add an instance with the same server UUID (" + server_uuid +
          ") of an active member of the cluster '" + instance_def.endpoint +
          "'. Please change the server UUID of the instance to add, all "
          "members must have a unique server UUID.");
    }
  }
}

void Cluster_impl::validate_server_id(
    const mysqlshdk::mysql::IInstance &target_instance) const {
  // Get the server_id of the target instance.
  uint32_t server_id = target_instance.get_server_id();

  // Get connection option for the metadata.
  std::shared_ptr<Instance> cluster_instance = get_cluster_server();
  Connection_options cluster_cnx_opt =
      cluster_instance->get_connection_options();

  // Get list of instances in the metadata
  std::vector<Instance_metadata> metadata_instances = get_instances();

  // Get and compare the server_id of all instances with the one of
  // the target instance.
  for (const Instance_metadata &instance : metadata_instances) {
    if (server_id != 0 && server_id == instance.server_id) {
      throw std::runtime_error{"The server_id '" + std::to_string(server_id) +
                               "' is already used by instance '" +
                               instance.endpoint + "'."};
    }
  }
}

std::vector<Instance_md_and_gr_member> Cluster_impl::get_instances_with_state(
    bool allow_offline) const {
  std::vector<std::pair<Instance_metadata, mysqlshdk::gr::Member>> ret;

  std::vector<mysqlshdk::gr::Member> members;

  if (get_cluster_server()) {
    try {
      members = mysqlshdk::gr::get_members(*get_cluster_server());
    } catch (const std::runtime_error &e) {
      if (!allow_offline ||
          !strstr(e.what(), "Group replication does not seem to be"))
        throw;
    }
  }

  std::vector<Instance_metadata> md = get_instances();

  for (const auto &i : md) {
    auto m = std::find_if(members.begin(), members.end(),
                          [&i](const mysqlshdk::gr::Member &member) {
                            return member.uuid == i.uuid;
                          });
    if (m != members.end()) {
      ret.push_back({i, *m});
    } else {
      mysqlshdk::gr::Member mm;
      mm.uuid = i.uuid;
      mm.state = mysqlshdk::gr::Member_state::MISSING;
      ret.push_back({i, mm});
    }
  }

  return ret;
}

std::unique_ptr<mysqlshdk::config::Config> Cluster_impl::create_config_object(
    const std::vector<std::string> &ignored_instances, bool skip_invalid_state,
    bool persist_only, bool best_effort, bool allow_cluster_offline) const {
  auto cfg = std::make_unique<mysqlshdk::config::Config>();

  auto console = mysqlsh::current_console();

  // Get all cluster instances, including state information to update
  // auto-increment values.
  std::vector<std::pair<Instance_metadata, mysqlshdk::gr::Member>>
      instance_defs = get_instances_with_state(allow_cluster_offline);

  for (const auto &instance_def : instance_defs) {
    // If instance is on the list of instances to be ignored, skip it.
    if (std::find_if(ignored_instances.begin(), ignored_instances.end(),
                     mysqlshdk::utils::Endpoint_predicate{
                         instance_def.first.endpoint}) !=
        ignored_instances.end()) {
      continue;
    }

    // Use the GR state hold by instance_def.state (but convert it to a proper
    // mysqlshdk::gr::Member_state to be handled properly).
    mysqlshdk::gr::Member_state state = instance_def.second.state;

    if (best_effort || state == mysqlshdk::gr::Member_state::ONLINE ||
        state == mysqlshdk::gr::Member_state::RECOVERING) {
      // Set login credentials to connect to instance.
      // NOTE: It is assumed that the same login credentials can be used to
      //       connect to all cluster instances.
      Connection_options instance_cnx_opts =
          shcore::get_connection_options(instance_def.first.endpoint, false);
      instance_cnx_opts.set_login_options_from(
          get_cluster_server()->get_connection_options());

      // Try to connect to instance.
      log_debug("Connecting to instance '%s'",
                instance_def.first.endpoint.c_str());
      std::shared_ptr<mysqlsh::dba::Instance> instance;
      try {
        instance = Instance::connect(instance_cnx_opts);
        log_debug("Successfully connected to instance");
      } catch (const shcore::Error &err) {
        if (best_effort) {
          console->print_warning(
              "The settings cannot be updated for instance '" +
              instance_def.first.endpoint + "': " + err.format());
          continue;
        } else {
          log_debug("Failed to connect to instance: %s", err.format().c_str());
          console->print_error(
              "Unable to connect to instance '" + instance_def.first.endpoint +
              "'. Please verify connection credentials and make sure the "
              "instance is available.");

          throw;
        }
      }

      // Determine if SET PERSIST is supported.
      std::optional<bool> support_set_persist =
          instance->is_set_persist_supported();
      mysqlshdk::mysql::Var_qualifier set_type =
          mysqlshdk::mysql::Var_qualifier::GLOBAL;
      bool skip = false;
      if (support_set_persist.value_or(false)) {
        set_type = persist_only ? mysqlshdk::mysql::Var_qualifier::PERSIST_ONLY
                                : mysqlshdk::mysql::Var_qualifier::PERSIST;

      } else {
        // If we want persist_only but it's not supported, we skip it since it
        // can't help us
        if (persist_only) skip = true;
      }

      // Add configuration handler for server.
      if (!skip) {
        cfg->add_handler(
            instance_def.first.endpoint,
            std::unique_ptr<mysqlshdk::config::IConfig_handler>(
                std::make_unique<mysqlshdk::config::Config_server_handler>(
                    instance, set_type)));
      }

      // Print a warning if SET PERSIST is not supported, for users to execute
      // dba.configureLocalInstance().
      if (!support_set_persist.has_value()) {
        console->print_warning(
            "Instance '" + instance_def.first.endpoint +
            "' cannot persist configuration since MySQL version " +
            instance->get_version().get_base() +
            " does not support the SET PERSIST command "
            "(MySQL version >= 8.0.11 required). Please use the dba." +
            "<<<configureLocalInstance>>>() command locally to persist the "
            "changes.");
      } else if (!support_set_persist.value()) {
        console->print_warning(
            "Instance '" + instance_def.first.endpoint +
            "' will not load the persisted cluster configuration upon reboot "
            "since 'persisted-globals-load' is set to 'OFF'. Please use the "
            "dba.<<<configureLocalInstance>>>"
            "() command locally to persist the changes or set "
            "'persisted-globals-load' to 'ON' on the configuration file.");
      }
    } else {
      // Ignore instance with an invalid state (i.e., do not issue an erro), if
      // skip_invalid_state is true.
      if (skip_invalid_state) continue;

      // Issue an error if the instance is not active.
      console->print_error("The settings cannot be updated for instance '" +
                           instance_def.first.endpoint +
                           "' because it is on a '" +
                           mysqlshdk::gr::to_string(state) + "' state.");

      throw shcore::Exception::runtime_error(
          "The instance '" + instance_def.first.endpoint + "' is '" +
          mysqlshdk::gr::to_string(state) + "'");
    }
  }

  return cfg;
}

void Cluster_impl::query_group_wide_option_values(
    mysqlshdk::mysql::IInstance *target_instance,
    std::optional<std::string> *out_gr_consistency,
    std::optional<int64_t> *out_gr_member_expel_timeout) const {
  auto console = mysqlsh::current_console();

  Option_info<std::string> gr_consistency;
  Option_info<int64_t> gr_member_expel_timeout;

  std::vector<std::string> skip_list;
  skip_list.push_back(target_instance->get_canonical_address());

  // loop though all members to check if there is any member that doesn't:
  // - have support for the group_replication_consistency option (null value)
  // or a member that doesn't have the default value. The default value
  // Eventual has the same behavior as members had before the option was
  // introduced. As such, having that value or having no support for the
  // group_replication_consistency is the same.
  // - have support for the group_replication_member_expel_timeout option
  // (null value) or a member that doesn't have the default value. The default
  // value 0 has the same behavior as members had before the option was
  // introduced. As such, having the 0 value or having no support for the
  // group_replication_member_expel_timeout is the same.
  execute_in_members({mysqlshdk::gr::Member_state::ONLINE,
                      mysqlshdk::gr::Member_state::RECOVERING},
                     get_cluster_server()->get_connection_options(), skip_list,
                     [&gr_consistency, &gr_member_expel_timeout](
                         const std::shared_ptr<Instance> &instance,
                         const mysqlshdk::gr::Member &) {
                       {
                         auto value = instance->get_sysvar_string(
                             "group_replication_consistency",
                             mysqlshdk::mysql::Var_qualifier::GLOBAL);

                         if (!value.has_value()) {
                           gr_consistency.found_not_supported = true;
                         } else if (*value != "EVENTUAL" && *value != "0") {
                           gr_consistency.found_non_default = true;
                           gr_consistency.non_default_value = *value;
                         }
                       }

                       {
                         auto value = instance->get_sysvar_int(
                             "group_replication_member_expel_timeout",
                             mysqlshdk::mysql::Var_qualifier::GLOBAL);

                         if (!value.has_value()) {
                           gr_member_expel_timeout.found_not_supported = true;
                         } else if (*value != 0) {
                           gr_member_expel_timeout.found_non_default = true;
                           gr_member_expel_timeout.non_default_value = *value;
                         }
                       }
                       // if we have found both a instance that doesnt have
                       // support for the option and an instance that doesn't
                       // have the default value, then we don't need to look at
                       // any other instance on the cluster.
                       return !(gr_consistency.found_not_supported &&
                                gr_consistency.found_non_default &&
                                gr_member_expel_timeout.found_not_supported &&
                                gr_member_expel_timeout.found_non_default);
                     });

  if (target_instance->get_version() < mysqlshdk::utils::Version(8, 0, 14)) {
    if (gr_consistency.found_non_default) {
      console->print_warning(
          "The " + gr_consistency.non_default_value +
          " consistency value of the cluster "
          "is not supported by the instance '" +
          target_instance->get_connection_options().uri_endpoint() +
          "' (version >= 8.0.14 is required). In single-primary mode, upon "
          "failover, the member with the lowest version is the one elected as"
          " primary.");
    }
  } else {
    if (out_gr_consistency) *out_gr_consistency = "EVENTUAL";

    if (gr_consistency.found_non_default) {
      // if we found any non default group_replication_consistency value, then
      // we use that value on the instance being added
      if (out_gr_consistency)
        *out_gr_consistency = gr_consistency.non_default_value;

      if (gr_consistency.found_not_supported) {
        console->print_warning(
            "The instance '" +
            target_instance->get_connection_options().uri_endpoint() +
            "' inherited the " + gr_consistency.non_default_value +
            " consistency value from the cluster, however some instances on "
            "the group do not support this feature (version < 8.0.14). In "
            "single-primary mode, upon failover, the member with the lowest "
            "version will be the one elected and it doesn't support this "
            "option.");
      }
    }
  }

  if (target_instance->get_version() < mysqlshdk::utils::Version(8, 0, 13)) {
    if (gr_member_expel_timeout.found_non_default) {
      console->print_warning(
          "The expelTimeout value of the cluster '" +
          std::to_string(gr_member_expel_timeout.non_default_value) +
          "' is not supported by the instance '" +
          target_instance->get_connection_options().uri_endpoint() +
          "' (version >= 8.0.13 is required). A member "
          "that doesn't have support for the expelTimeout option has the "
          "same behavior as a member with expelTimeout=0.");
    }
  } else {
    if (out_gr_member_expel_timeout) *out_gr_member_expel_timeout = 0;

    if (gr_member_expel_timeout.found_non_default) {
      // if we found any non default group_replication_member_expel_timeout
      // value, then we use that value on the instance being added
      if (out_gr_member_expel_timeout)
        *out_gr_member_expel_timeout =
            gr_member_expel_timeout.non_default_value;

      if (gr_member_expel_timeout.found_not_supported) {
        console->print_warning(
            "The instance '" +
            target_instance->get_connection_options().uri_endpoint() +
            "' inherited the '" +
            std::to_string(gr_member_expel_timeout.non_default_value) +
            "' consistency value from the cluster, however some instances on "
            "the group do not support this feature (version < 8.0.13). There "
            "is a possibility that the cluster member (killer node), "
            "responsible for expelling the member suspected of having "
            "failed, does not support the expelTimeout option. In "
            "this case the behavior would be the same as if having "
            "expelTimeout=0.");
      }
    }
  }
}

void Cluster_impl::update_group_members_for_removed_member(
    const std::string &server_uuid) {
  // Get the Cluster Config Object
  auto cfg = create_config_object({}, true);

  // Iterate through all ONLINE and RECOVERING cluster members and update
  // their group_replication_group_seeds value by removing the
  // gr_local_address of the instance that was removed
  log_debug("Updating group_replication_group_seeds of cluster members");
  {
    auto group_seeds = get_cluster_group_seeds();
    if (group_seeds.find(server_uuid) != group_seeds.end()) {
      group_seeds.erase(server_uuid);
    }
    mysqlshdk::gr::update_group_seeds(cfg.get(), group_seeds);
  }
  cfg->apply();

  // Update the auto-increment values
  {
    // Auto-increment values must be updated according to:
    //
    // Set auto-increment for single-primary topology:
    // - auto_increment_increment = 1
    // - auto_increment_offset = 2
    //
    // Set auto-increment for multi-primary topology:
    // - auto_increment_increment = n;
    // - auto_increment_offset = 1 + server_id % n;
    // where n is the size of the GR group if > 7, otherwise n = 7.
    //
    // We must update the auto-increment values in Remove_instance for 2
    // scenarios
    //   - Multi-primary Cluster
    //   - Cluster that had more 7 or more members before the
    //   Remove_instance
    //     operation
    //
    // NOTE: in the other scenarios, the Add_instance operation is in charge
    // of updating auto-increment accordingly

    mysqlshdk::gr::Topology_mode topology_mode =
        get_metadata_storage()->get_cluster_topology_mode(get_id());

    // Get the current number of members of the Cluster
    uint64_t cluster_member_count =
        get_metadata_storage()->get_cluster_size(get_id());

    // we really only need to update auto_increment if we were above 7
    // (otherwise is always 7), but since the member was already removed, the
    // condition must be >= 7 (to catch were be moved from 8 to 7)
    if ((topology_mode == mysqlshdk::gr::Topology_mode::MULTI_PRIMARY) &&
        (cluster_member_count >= 7)) {
      // Call update_auto_increment to do the job in all instances
      mysqlshdk::gr::update_auto_increment(
          cfg.get(), mysqlshdk::gr::Topology_mode::MULTI_PRIMARY);

      cfg->apply();
    }
  }
}

void Cluster_impl::add_instance(
    const mysqlshdk::db::Connection_options &instance_def,
    const cluster::Add_instance_options &options) {
  check_preconditions("addInstance");

  Scoped_instance target(connect_target_instance(instance_def, true, true));

  // put an exclusive lock on the cluster
  auto c_lock = get_lock_exclusive();

  // put an exclusive lock on the target instance
  auto i_lock = target->get_lock_exclusive();

  Cluster_topology_executor<cluster::Add_instance>{this, target, options}.run();

  // Verification step to ensure the server_id is an attribute on all the
  // instances of the cluster
  // TODO(miguel): there should be some tag in the metadata to know whether
  // server_id is already stored or not, to avoid connecting to all members
  // each time to do so. Also, this should probably be done at the beginning.
  ensure_metadata_has_server_id(target);
}

void Cluster_impl::rejoin_instance(
    const Connection_options &instance_def,
    const cluster::Rejoin_instance_options &options) {
  check_preconditions("rejoinInstance");

  Scoped_instance target(connect_target_instance(instance_def, true));

  // put a shared lock on the cluster
  auto c_lock = get_lock_shared();

  // put an exclusive lock on the target instance
  auto i_lock = target->get_lock_exclusive();

  Cluster_topology_executor<cluster::Rejoin_instance>{this, target, options}
      .run();

  // Verification step to ensure the server_id is an attribute on all the
  // instances of the cluster
  ensure_metadata_has_server_id(target);
}

void Cluster_impl::remove_instance(
    const Connection_options &instance_def,
    const cluster::Remove_instance_options &options) {
  check_preconditions("removeInstance");

  // put an exclusive lock on the cluster
  auto c_lock = get_lock_exclusive();

  Cluster_topology_executor<cluster::Remove_instance>{this, instance_def,
                                                      options}
      .run();
}

std::shared_ptr<Instance> connect_to_instance_for_metadata(
    const mysqlshdk::db::Connection_options &instance_definition) {
  std::string instance_address =
      instance_definition.as_uri(mysqlshdk::db::uri::formats::only_transport());

  log_debug("Connecting to '%s' to query for metadata information...",
            instance_address.c_str());

  std::shared_ptr<Instance> classic;
  try {
    classic = Instance::connect(instance_definition,
                                current_shell_options()->get().wizards);
  } catch (const shcore::Error &e) {
    std::stringstream ss;
    ss << "Error opening session to '" << instance_address << "': " << e.what();
    log_warning("%s", ss.str().c_str());

    // TODO(alfredo) - this should be validated before adding the instance
    // Check if we're adopting a GR cluster, if so, it could happen that
    // we can't connect to it because root@localhost exists but root@hostname
    // doesn't (GR keeps the hostname in the members table)
    if (e.code() == ER_ACCESS_DENIED_ERROR) {
      std::stringstream se;
      se << "Access denied connecting to instance " << instance_address << ".\n"
         << "Please ensure all instances in the same group/cluster have"
            " the same password for account '"
         << instance_definition.get_user()
         << "' and that it is accessible from the host mysqlsh is running "
            "from.";
      throw shcore::Exception::runtime_error(se.str());
    }
    throw shcore::Exception::runtime_error(ss.str());
  }

  return classic;
}

void Cluster_impl::add_metadata_for_instance(
    const mysqlshdk::db::Connection_options &instance_definition,
    const std::string &label) const {
  log_debug("Adding instance to metadata");

  auto instance = connect_to_instance_for_metadata(instance_definition);

  add_metadata_for_instance(*instance, label);
}

void Cluster_impl::add_metadata_for_instance(
    const mysqlshdk::mysql::IInstance &instance,
    const std::string &label) const {
  try {
    Instance_metadata instance_md(query_instance_info(instance, true));

    if (!label.empty()) instance_md.label = label;

    instance_md.cluster_id = get_id();

    // Add the instance to the metadata.
    get_metadata_storage()->insert_instance(instance_md);
  } catch (const shcore::Exception &exp) {
    check_gr_empty_local_address_exception(exp, instance);
    throw;
  }
}

void Cluster_impl::update_metadata_for_instance(
    const mysqlshdk::db::Connection_options &instance_definition,
    Instance_id instance_id, const std::string &label) const {
  auto instance = connect_to_instance_for_metadata(instance_definition);

  update_metadata_for_instance(*instance, instance_id, label);
}

void Cluster_impl::update_metadata_for_instance(
    const mysqlshdk::mysql::IInstance &instance, Instance_id instance_id,
    const std::string &label) const {
  log_debug("Updating instance metadata");

  auto console = mysqlsh::current_console();
  console->print_info("Updating instance metadata...");

  try {
    Instance_metadata instance_md(query_instance_info(instance, true));

    instance_md.cluster_id = get_id();
    instance_md.id = instance_id;
    // note: label is only updated in MD if <> ''
    instance_md.label = label;

    // Updates the instance metadata.
    get_metadata_storage()->update_instance(instance_md);

    console->print_info("The instance metadata for '" +
                        instance.get_canonical_address() +
                        "' was successfully updated.");
    console->print_info();
  } catch (const shcore::Exception &exp) {
    check_gr_empty_local_address_exception(exp, instance);
    throw;
  }
}

void Cluster_impl::remove_instance_metadata(
    const mysqlshdk::db::Connection_options &instance_def) {
  log_debug("Removing instance from metadata");

  std::string port = std::to_string(instance_def.get_port());

  std::string host = instance_def.get_host();

  // Check if the instance was already added
  std::string instance_address = host + ":" + port;

  get_metadata_storage()->remove_instance(instance_address);
}

mysqlshdk::mysql::Auth_options
Cluster_impl::refresh_clusterset_replication_user() const {
  mysqlshdk::mysql::Auth_options creds;
  std::string repl_user, repl_user_host;

  // Get the replication account in use
  std::tie(repl_user, repl_user_host) =
      get_metadata_storage()->get_cluster_repl_account(get_id());

  creds.user = repl_user;

  try {
    auto console = mysqlsh::current_console();

    log_info("Resetting password for %s@%% at %s", creds.user.c_str(),
             m_primary_master->descr().c_str());

    // Change the replication user to use the newly generated password
    std::string repl_password;
    mysqlshdk::mysql::set_random_password(*get_primary_master(), creds.user,
                                          {repl_user_host}, &repl_password);
    creds.password = repl_password;
  } catch (const std::exception &e) {
    throw shcore::Exception::runtime_error(shcore::str_format(
        "Error while resetting password for replication account: %s",
        e.what()));
  }

  return creds;
}

void Cluster_impl::update_replication_allowed_host(const std::string &host) {
  bool upgraded = false;
  auto primary = get_primary_master();

retry:
  for (const Instance_metadata &instance : get_instances()) {
    auto account = get_metadata_storage()->get_instance_repl_account(
        instance.uuid, Cluster_type::GROUP_REPLICATION);

    if (account.first.empty()) {
      if (!upgraded) {
        upgraded = true;
        current_console()->print_note(
            "Legacy cluster account management detected, will update it "
            "first.");
        ensure_metadata_has_recovery_accounts();
        goto retry;
      } else {
        throw shcore::Exception::runtime_error(
            "Unable to perform account management upgrade for the cluster.");
      }
    } else if (account.second != host) {
      log_info("Re-creating account for %s: %s@%s -> %s@%s",
               instance.endpoint.c_str(), account.first.c_str(),
               account.second.c_str(), account.first.c_str(), host.c_str());
      clone_user(*primary, account.first, account.second, account.first, host);

      // drop all (other) hosts in case the account was created at an old
      // version with multiple accounts per user
      auto hosts =
          mysqlshdk::mysql::get_all_hostnames_for_user(*primary, account.first);
      for (const auto &h : hosts) {
        if (host != h) primary->drop_user(account.first, h, true);
      }

      get_metadata_storage()->update_instance_repl_account(
          instance.uuid, Cluster_type::GROUP_REPLICATION, account.first, host);
    } else {
      log_info("Skipping account recreation for %s: %s@%s == %s@%s",
               instance.endpoint.c_str(), account.first.c_str(),
               account.second.c_str(), account.first.c_str(), host.c_str());
    }
  }
}

void Cluster_impl::configure_cluster_set_member(
    const std::shared_ptr<mysqlsh::dba::Instance> &target_instance) const {
  try {
    target_instance->set_sysvar("skip_replica_start", true,
                                mysqlshdk::mysql::Var_qualifier::PERSIST_ONLY);
  } catch (const mysqlshdk::db::Error &e) {
    throw shcore::Exception::mysql_error_with_code_and_state(e.what(), e.code(),
                                                             e.sqlstate());
  }

  if (!is_primary_cluster()) {
    auto console = mysqlsh::current_console();

    console->print_info(
        "* Waiting for the Cluster to synchronize with the PRIMARY Cluster...");
    sync_transactions(*get_cluster_server(), k_clusterset_async_channel_name,
                      0);

    auto cs = get_cluster_set_object();

    auto ar_options = cs->get_clusterset_replication_options();

    ar_options.repl_credentials = refresh_clusterset_replication_user();

    // Update the credentials on all cluster members
    execute_in_members({}, get_primary_master()->get_connection_options(), {},
                       [&](const std::shared_ptr<Instance> &target,
                           const mysqlshdk::gr::Member &) {
                         async_update_replica_credentials(
                             target.get(), k_clusterset_async_channel_name,
                             ar_options, false);
                         return true;
                       });

    // Setup the replication channel at the target instance but do not start
    // it since that's handled by Group Replication
    console->print_info(
        "* Configuring ClusterSet managed replication channel...");

    async_add_replica(get_primary_master().get(), target_instance.get(),
                      k_clusterset_async_channel_name, ar_options, true, false,
                      false);

    console->print_info();
  }
}

void Cluster_impl::restore_recovery_account_all_members(
    bool reset_password) const {
  // if we don't need to reset the password (for example the recovery accounts
  // only use certificates), all we need to do is to update the replication
  // credentials
  if (!reset_password) {
    log_info("Restoring recovery accounts with certificates only.");

    execute_in_members(
        [this](const std::shared_ptr<Instance> &instance,
               const Instance_md_and_gr_member &info) {
          if (info.second.state != mysqlshdk::gr::Member_state::ONLINE)
            return true;

          auto [recovery_user, recovery_host] =
              get_metadata_storage()->get_instance_repl_account(
                  instance->get_uuid(), Cluster_type::GROUP_REPLICATION);

          mysqlshdk::mysql::Replication_credentials_options options;

          mysqlshdk::mysql::change_replication_credentials(
              *instance, mysqlshdk::gr::k_gr_recovery_channel, recovery_user,
              options);

          return true;
        },
        [](const shcore::Error &, const Instance_md_and_gr_member &) {
          return true;
        });

    return;
  }

  execute_in_members(
      [this](const std::shared_ptr<Instance> &instance,
             const Instance_md_and_gr_member &info) {
        if (info.second.state != mysqlshdk::gr::Member_state::ONLINE)
          return true;

        try {
          reset_recovery_password(instance);
        } catch (const std::exception &e) {
          // If we can't change the recovery account password for some
          // reason, we must re-create it
          log_info(
              "Unable to reset the recovery password of instance '%s', "
              "re-creating a new account: %s",
              instance->descr().c_str(), e.what());

          auto cert_subject =
              query_cluster_instance_auth_cert_subject(*instance);

          recreate_replication_user(instance, cert_subject);
        }

        return true;
      },
      [](const shcore::Error &, const Instance_md_and_gr_member &) {
        return true;
      });
}

void Cluster_impl::change_recovery_credentials_all_members(
    const mysqlshdk::mysql::Auth_options &repl_account) const {
  execute_in_members(
      [&repl_account](const std::shared_ptr<Instance> &instance,
                      const Instance_md_and_gr_member &info) {
        if (info.second.state != mysqlshdk::gr::Member_state::ONLINE)
          return true;

        try {
          mysqlshdk::mysql::Replication_credentials_options options;
          options.password = repl_account.password.value_or("");

          mysqlshdk::mysql::change_replication_credentials(
              *instance, mysqlshdk::gr::k_gr_recovery_channel,
              repl_account.user, options);
        } catch (const std::exception &e) {
          current_console()->print_warning(shcore::str_format(
              "Error updating recovery credentials for %s: %s",
              instance->descr().c_str(), e.what()));
        }

        return true;
      },
      [](const shcore::Error &, const Instance_md_and_gr_member &) {
        return true;
      });
}

void Cluster_impl::create_local_replication_user(
    const std::shared_ptr<mysqlsh::dba::Instance> &target_instance,
    std::string_view auth_cert_subject,
    const Group_replication_options &gr_options,
    bool propagate_credentials_donors) {
  // When using the 'MySQL' communication stack, the account must already
  // exist in the joining member since authentication is based on MySQL
  // credentials so the account must exist in both ends before GR
  // starts. For those reasons, we must create the account with binlog
  // disabled before starting GR, otherwise we'd create errant
  // transaction
  mysqlshdk::mysql::Suppress_binary_log nobinlog(target_instance.get());

  if (target_instance->get_sysvar_bool("super_read_only", false)) {
    target_instance->set_sysvar("super_read_only", false);
  }

  mysqlshdk::mysql::Auth_options repl_account;

  std::tie(repl_account, std::ignore) =
      create_replication_user(target_instance.get(), auth_cert_subject, true,
                              gr_options.recovery_credentials.value_or(
                                  mysqlshdk::mysql::Auth_options{}),
                              false);

  // Change GR's recovery replication credentials in all possible
  // donors so they are able to connect to the joining member. GR will pick a
  // suitable donor (that we cannot determine) and it needs to be able to
  // connect and authenticate to the joining member.
  // NOTE: Instances in RECOVERING must be skipped since won't be used as donor
  // and the change source command would fail anyway
  if (propagate_credentials_donors)
    change_recovery_credentials_all_members(repl_account);

  DBUG_EXECUTE_IF("fail_recovery_mysql_stack", {
    // Revoke the REPLICATION_SLAVE to make the distributed recovery fail
    auto primary = get_primary_master();
    primary->execute("REVOKE REPLICATION SLAVE on *.* from " +
                     repl_account.user);
    target_instance->execute("REVOKE REPLICATION SLAVE on *.* from " +
                             repl_account.user);
  });
}

void Cluster_impl::create_replication_users_at_instance(
    const std::shared_ptr<mysqlsh::dba::Instance> &target_instance) {
  // When using the 'MySQL' communication stack and the recovery accounts
  // require certificates, we need to make sure that every account for all
  // current members also exists in the target (new) instance. To avoid having
  // errant transactions when GR starts, we do this with binlog disabled.
  mysqlshdk::mysql::Suppress_binary_log nobinlog(target_instance.get());

  if (target_instance->get_sysvar_bool("super_read_only", false))
    target_instance->set_sysvar("super_read_only", false);

  execute_in_members(
      [this, &target_instance](const std::shared_ptr<Instance> &instance,
                               const Instance_md_and_gr_member &info) {
        if ((info.second.state != mysqlshdk::gr::Member_state::ONLINE) &&
            (info.second.state != mysqlshdk::gr::Member_state::RECOVERING))
          return true;

        std::tuple<std::string, std::vector<std::string>, bool> user;
        user = get_replication_user(*instance);

        for (const auto &host : std::get<1>(user)) {
          if (target_instance->user_exists(std::get<0>(user), host)) {
            log_info(
                "User '%s'@'%s' already existed at instance '%s'. It will be "
                "deleted and created again.",
                std::get<0>(user).c_str(), host.c_str(),
                target_instance->descr().c_str());

            target_instance->drop_user(std::get<0>(user), host);
          }

          log_info("Copying user '%s'@'%s' to instance '%s'.",
                   std::get<0>(user).c_str(), host.c_str(),
                   target_instance->descr().c_str());
          clone_user(*instance, *target_instance, std::get<0>(user), host);
        }

        return true;
      },
      [](const shcore::Error &, const Instance_md_and_gr_member &) {
        return true;
      });
}

void Cluster_impl::update_group_peers(
    const mysqlshdk::mysql::IInstance &target_instance,
    const Group_replication_options &gr_options, int cluster_member_count,
    const std::string &self_address, bool group_seeds_only) {
  assert(cluster_member_count > 0);

  // Get the gr_address of the instance being added
  std::string added_instance_gr_address = *gr_options.local_address;

  // Create a configuration object for the cluster, ignoring the added
  // instance, to update the remaining cluster members.
  // NOTE: only members that already belonged to the cluster and are either
  //       ONLINE or RECOVERING will be considered.
  std::vector<std::string> ignore_instances_vec = {self_address};
  std::unique_ptr<mysqlshdk::config::Config> cluster_cfg =
      create_config_object(ignore_instances_vec, true);

  // Update the group_replication_group_seeds of the cluster members
  // by adding the gr_local_address of the instance that was just added.
  log_debug("Updating Group Replication seeds on all active members...");
  {
    auto group_seeds = get_cluster_group_seeds();
    assert(!added_instance_gr_address.empty());
    group_seeds[target_instance.get_uuid()] = added_instance_gr_address;
    mysqlshdk::gr::update_group_seeds(cluster_cfg.get(), group_seeds);
  }

  cluster_cfg->apply();

  if (!group_seeds_only) {
    // Increase the cluster_member_count counter
    cluster_member_count++;

    // Auto-increment values must be updated according to:
    //
    // Set auto-increment for single-primary topology:
    // - auto_increment_increment = 1
    // - auto_increment_offset = 2
    //
    // Set auto-increment for multi-primary topology:
    // - auto_increment_increment = n;
    // - auto_increment_offset = 1 + server_id % n;
    // where n is the size of the GR group if > 7, otherwise n = 7.
    //
    // We must update the auto-increment values in Cluster_join for 2
    // scenarios
    //   - Multi-primary Cluster
    //   - Cluster that has 7 or more members after the Cluster_join
    //     operation
    //
    // NOTE: in the other scenarios, the Cluster_join operation is in
    // charge of updating auto-increment accordingly

    // Get the topology mode of the replicaSet
    mysqlshdk::gr::Topology_mode topology_mode =
        get_metadata_storage()->get_cluster_topology_mode(get_id());

    if (topology_mode == mysqlshdk::gr::Topology_mode::MULTI_PRIMARY &&
        cluster_member_count > 7) {
      log_debug("Updating auto-increment settings on all active members...");
      mysqlshdk::gr::update_auto_increment(
          cluster_cfg.get(), mysqlshdk::gr::Topology_mode::MULTI_PRIMARY,
          cluster_member_count);
      cluster_cfg->apply();
    }
  }
}

void Cluster_impl::check_instance_configuration(
    const std::shared_ptr<mysqlsh::dba::Instance> &target_instance,
    bool using_clone_recovery, checks::Check_type checks_type,
    bool already_member) const {
  // Check instance version compatibility with cluster.
  cluster_topology_executor_ops::ensure_instance_check_installed_schema_version(
      target_instance, get_lowest_instance_version());

  // Check instance configuration and state, like dba.checkInstance
  // But don't do it if it was already done by the caller
  ensure_gr_instance_configuration_valid(target_instance.get(), true,
                                         using_clone_recovery);

  // Validate the lower_case_table_names and default_table_encryption
  // variables. Their values must be the same on the target instance as they
  // are on the cluster.

  // The lower_case_table_names can only be set the first time the server
  // boots, as such there is no need to validate it other than the first time
  // the instance is added to the cluster.
  validate_variable_compatibility(*target_instance, "lower_case_table_names");

  // The default_table_encryption is a dynamic variable, so we validate it on
  // the Cluster_join and on the rejoin operation. The reboot operation does a
  // rejoin in the background, so running the check on the rejoin will cover
  // both operations.
  if (get_lowest_instance_version() >= mysqlshdk::utils::Version(8, 0, 16) &&
      target_instance->get_version() >= mysqlshdk::utils::Version(8, 0, 16)) {
    validate_variable_compatibility(*target_instance,
                                    "default_table_encryption");
  }

  // Verify if the instance is running asynchronous
  // replication
  // NOTE: Verify for all operations: addInstance(), rejoinInstance() and
  // rebootClusterFromCompleteOutage()
  checks::validate_async_channels(
      *target_instance,
      is_cluster_set_member()
          ? std::unordered_set<std::string>{k_clusterset_async_channel_name}
          : std::unordered_set<std::string>{},
      checks_type);

  if (!already_member) {
    // Check instance server UUID (must be unique among the cluster members).
    validate_server_uuid(*target_instance);

    // Ensure instance server ID is unique among the cluster members.
    try {
      validate_server_id(*target_instance);
    } catch (const std::runtime_error &err) {
      auto console = mysqlsh::current_console();
      console->print_error(
          "Cannot join instance '" + target_instance->descr() +
          "' to the cluster because it has the same server ID "
          "of a member of the cluster. Please change the server "
          "ID of the instance to add: all members must have a "
          "unique server ID.");
      throw;
    }
  }
}

shcore::Value Cluster_impl::describe() {
  // Throw an error if the cluster has already been dissolved

  check_preconditions("describe");

  return cluster_describe();
}

shcore::Value Cluster_impl::cluster_describe() {
  // Create the Cluster_describe command and execute it.
  cluster::Describe op_describe(*this);
  // Always execute finish when leaving "try catch".
  auto finally =
      shcore::on_leave_scope([&op_describe]() { op_describe.finish(); });
  // Prepare the Cluster_describe command execution (validations).
  op_describe.prepare();
  // Execute Cluster_describe operations.
  return op_describe.execute();
}

Cluster_status Cluster_impl::cluster_status(int *out_num_failures_tolerated,
                                            int *out_num_failures) const {
  int total_members =
      static_cast<int>(m_metadata_storage->get_cluster_size(get_id()));

  if (!m_primary_master) {
    if (out_num_failures_tolerated) *out_num_failures_tolerated = 0;
    if (out_num_failures) *out_num_failures = total_members;

    switch (cluster_availability()) {
      case Cluster_availability::ONLINE:
      case Cluster_availability::ONLINE_NO_PRIMARY:
        // this can happen if we're in a clusterset and didn't acquire the
        // global primary
        break;
      case Cluster_availability::OFFLINE:
        return Cluster_status::OFFLINE;
      case Cluster_availability::NO_QUORUM:
        return Cluster_status::NO_QUORUM;
      case Cluster_availability::SOME_UNREACHABLE:
      case Cluster_availability::UNREACHABLE:
        return Cluster_status::UNKNOWN;
    }
  }

  auto target = get_cluster_server();

  shcore::Dictionary_t dict = shcore::make_dict();

  bool single_primary = false;
  bool has_quorum = false;

  // If the primary went OFFLINE for some reason, we must use the target
  // instance to obtain the group membership info.
  //
  // A possible scenario on which that occur is when the primary is
  // auto-rejoining. other members may not have noticed yet so they still
  // consider it as the primary from their point of view. However, the instance
  // itself is still OFFLINE so the group membership information cannot be
  // queried.
  if (!mysqlshdk::gr::is_active_member(*target) ||
      mysqlshdk::gr::is_running_gr_auto_rejoin(*target) ||
      mysqlshdk::gr::is_group_replication_delayed_starting(*target)) {
    target = get_cluster_server();
  }

  std::vector<mysqlshdk::gr::Member> members;

  try {
    members = mysqlshdk::gr::get_members(*target, &single_primary, &has_quorum,
                                         nullptr);
  } catch (const std::exception &e) {
    if (shcore::str_beginswith(
            e.what(), "Group replication does not seem to be active")) {
      return Cluster_status::OFFLINE;
    } else {
      throw;
    }
  }

  int num_online = 0;
  for (const auto &m : members) {
    if (m.state == mysqlshdk::gr::Member_state::ONLINE) {
      num_online++;
    }
  }

  int number_of_failures_tolerated = num_online > 0 ? (num_online - 1) / 2 : 0;

  if (out_num_failures_tolerated) {
    *out_num_failures_tolerated = number_of_failures_tolerated;
  }
  if (out_num_failures) *out_num_failures = total_members - num_online;

  if (!has_quorum) {
    return Cluster_status::NO_QUORUM;
  } else {
    if (num_online > 0 && is_fenced_from_writes()) {
      return Cluster_status::FENCED_WRITES;
    } else if (total_members > num_online) {
      // partial, some members are not online
      if (number_of_failures_tolerated == 0) {
        return Cluster_status::OK_NO_TOLERANCE_PARTIAL;
      } else {
        return Cluster_status::OK_PARTIAL;
      }
    } else {
      if (number_of_failures_tolerated == 0) {
        return Cluster_status::OK_NO_TOLERANCE;
      } else {
        return Cluster_status::OK;
      }
    }
  }
}

shcore::Value Cluster_impl::cluster_status(int64_t extended) {
  // Create the Cluster_status command and execute it.
  cluster::Status op_status(*this, extended);
  // Always execute finish when leaving "try catch".
  auto finally = shcore::on_leave_scope([&op_status]() { op_status.finish(); });
  // Prepare the Cluster_status command execution (validations).
  op_status.prepare();
  // Execute Cluster_status operations.
  return op_status.execute();
}

shcore::Value Cluster_impl::status(int64_t extended) {
  // Throw an error if the cluster has already been dissolved
  check_preconditions("status");

  return cluster_status(extended);
}

shcore::Value Cluster_impl::list_routers(bool only_upgrade_required) {
  check_preconditions("listRouters");

  if (is_cluster_set_member()) {
    current_console()->print_error(
        "Cluster '" + get_name() + "' is a member of ClusterSet '" +
        get_cluster_set_object()->get_name() +
        "', use <ClusterSet>.<<<listRouters>>>() to list "
        "the ClusterSet Router instances");
    throw shcore::Exception::runtime_error(
        "Function not available for ClusterSet members");
  }

  shcore::Dictionary_t dict = shcore::make_dict();

  (*dict)["clusterName"] = shcore::Value(get_name());
  (*dict)["routers"] = router_list(get_metadata_storage().get(), get_id(),
                                   only_upgrade_required);

  return shcore::Value(dict);
}

void Cluster_impl::remove_router_metadata(const std::string &router) {
  // put a shared lock on the cluster
  auto c_lock = get_lock_shared();

  check_preconditions("removeRouterMetadata");

  Base_cluster_impl::remove_router_metadata(router);
}

void Cluster_impl::reset_recovery_password(const mysqlshdk::null_bool &force,
                                           const bool interactive) {
  check_preconditions("resetRecoveryAccountsPassword");

  // put an exclusive lock on the cluster
  auto c_lock = get_lock_exclusive();

  // Create the reset_recovery_command.
  cluster::Reset_recovery_accounts_password op_reset(interactive, force, *this);

  // Always execute finish
  shcore::on_leave_scope finally([&op_reset]() { op_reset.finish(); });

  // prepare and execute
  op_reset.prepare();
  op_reset.execute();
}

void Cluster_impl::enable_super_read_only_globally() const {
  auto console = mysqlsh::current_console();
  // Enable super_read_only on the primary member
  auto primary = get_cluster_server();

  if (!primary->get_sysvar_bool("super_read_only", false)) {
    console->print_info("* Enabling super_read_only on '" + primary->descr() +
                        "'...");
    primary->set_sysvar("super_read_only", true);
  }

  // Enable super_read_only on the remaining members
  using mysqlshdk::gr::Member_state;
  execute_in_members({Member_state::ONLINE, Member_state::RECOVERING},
                     get_cluster_server()->get_connection_options(),
                     {primary->descr()},
                     [=](const std::shared_ptr<Instance> &instance,
                         const mysqlshdk::gr::Member &) {
                       console->print_info("* Enabling super_read_only on '" +
                                           instance->descr() + "'...");
                       instance->set_sysvar("super_read_only", true);
                       return true;
                     });
}

void Cluster_impl::fence_all_traffic() {
  check_preconditions("fenceAllTraffic");

  auto console = mysqlsh::current_console();
  console->print_info("The Cluster '" + get_name() +
                      "' will be fenced from all traffic");
  console->print_info();

  // put an exclusive lock on the cluster
  auto c_lock = get_lock_exclusive();

  // To fence all traffic, the command must:
  //  1. enable super_read_only and offline on the primary member
  //  2. enable offline_mode and stop Group Replication on all secondaries
  //  3. stop Group Replication on the primary

  // Enable super_read_only and off_line on the primary member
  auto primary = get_cluster_server();
  console->print_info("* Enabling super_read_only on the primary '" +
                      primary->descr() + "'...");
  primary->set_sysvar("super_read_only", true);

  console->print_info("* Enabling offline_mode on the primary '" +
                      primary->descr() + "'...");
  primary->set_sysvar("offline_mode", true);

  // Enable super_read_only, offline_mode and stop GR on all secondaries
  using mysqlshdk::gr::Member_state;
  execute_in_members(
      {Member_state::ONLINE, Member_state::RECOVERING},
      get_cluster_server()->get_connection_options(),
      {get_cluster_server()->descr()},
      [&console](const std::shared_ptr<Instance> &instance,
                 const mysqlshdk::gr::Member &) {
        // Every secondary should be already super_read_only
        if (!instance->get_sysvar_bool("super_read_only", false)) {
          console->print_info("* Enabling super_read_only on '" +
                              instance->descr() + "'...");
          instance->set_sysvar("super_read_only", true);
        }

        console->print_info("* Enabling offline_mode on '" + instance->descr() +
                            "'...");
        instance->set_sysvar("offline_mode", true);

        console->print_info("* Stopping Group Replication on '" +
                            instance->descr() + "'...");
        mysqlshdk::gr::stop_group_replication(*instance);

        return true;
      });

  // Stop Group Replication on the primary member
  console->print_info("* Stopping Group Replication on the primary '" +
                      primary->descr() + "'...");
  mysqlshdk::gr::stop_group_replication(*primary);

  console->print_info();
  console->print_info("Cluster successfully fenced from all traffic");
}

void Cluster_impl::fence_writes() {
  check_preconditions("fenceWrites");

  auto console = mysqlsh::current_console();

  console->print_info("The Cluster '" + get_name() +
                      "' will be fenced from write traffic");
  console->print_info();

  // put an exclusive lock on the cluster
  auto c_lock = get_lock_exclusive();

  // Disable the GR member action mysql_disable_super_read_only_if_primary
  auto primary = get_cluster_server();

  console->print_info(
      "* Disabling automatic super_read_only management on the Cluster...");
  mysqlshdk::gr::disable_member_action(
      *primary, mysqlshdk::gr::k_gr_disable_super_read_only_if_primary,
      mysqlshdk::gr::k_gr_member_action_after_primary_election);

  // Enable super_read_only on the primary member first and after all remaining
  // members
  enable_super_read_only_globally();

  console->print_info();

  if (is_cluster_set_member() && is_primary_cluster()) {
    console->print_note(
        "Applications will now be blocked from performing writes on Cluster '" +
        get_name() +
        "'. Use <Cluster>.<<<unfenceWrites>>>() to resume writes if you're "
        "certain a split-brain is not in effect.");
  }

  console->print_info();
  console->print_info("Cluster successfully fenced from write traffic");
}

void Cluster_impl::unfence_writes() {
  check_preconditions("unfenceWrites");

  auto console = mysqlsh::current_console();

  console->print_info("The Cluster '" + get_name() +
                      "' will be unfenced from write traffic");
  console->print_info();

  // put an exclusive lock on the cluster
  auto c_lock = get_lock_exclusive();

  auto primary = get_cluster_server();

  // Check if the Cluster belongs to a ClusterSet and is a REPLICA Cluster, it
  // must be forbidden to unfence from write traffic REPLICA Clusters.
  if (is_cluster_set_member() && !is_primary_cluster()) {
    auto cluster_name = get_name();
    console->print_error(
        "Unable to unfence Cluster from write traffic: operation not permitted "
        "on REPLICA Clusters");

    throw shcore::Exception("The Cluster '" + cluster_name +
                                "' is a REPLICA Cluster of the ClusterSet '" +
                                get_cluster_set_object()->get_name() + "'",
                            SHERR_DBA_UNSUPPORTED_CLUSTER_TYPE);
  }

  // Check if the Cluster is fenced from Write traffic
  if (!is_fenced_from_writes()) {
    throw shcore::Exception("Cluster not fenced to write traffic",
                            SHERR_DBA_CLUSTER_NOT_FENCED);
  }

  // Check if offline_mode is enabled and disable it on all members
  using mysqlshdk::gr::Member_state;
  execute_in_members({Member_state::ONLINE, Member_state::RECOVERING},
                     get_cluster_server()->get_connection_options(), {},
                     [&console](const std::shared_ptr<Instance> &instance,
                                const mysqlshdk::gr::Member &) {
                       if (instance->get_sysvar_bool("offline_mode", false)) {
                         console->print_info("* Disabling 'offline_mode' on '" +
                                             instance->descr() + "'...");
                         instance->set_sysvar("offline_mode", false);
                       }
                       return true;
                     });

  // Enable the GR member action mysql_disable_super_read_only_if_primary
  console->print_info(
      "* Enabling automatic super_read_only management on the Cluster...");
  mysqlshdk::gr::enable_member_action(
      *primary, mysqlshdk::gr::k_gr_disable_super_read_only_if_primary,
      mysqlshdk::gr::k_gr_member_action_after_primary_election);

  // Disable super_read_only on the primary member
  if (primary->get_sysvar_bool("super_read_only", false)) {
    console->print_info("* Disabling super_read_only on the primary '" +
                        primary->descr() + "'...");
    primary->set_sysvar("super_read_only", false);
  }

  console->print_info();
  console->print_info("Cluster successfully unfenced from write traffic");
}

bool Cluster_impl::is_fenced_from_writes() const {
  if (!m_cluster_server) {
    return false;
  }

  auto primary = m_cluster_server;

  // Fencing is only supported for versions >= 8.0.27 with the introduction of
  // ClusterSet
  if (primary->get_version() < Precondition_checker::k_min_cs_version) {
    return false;
  }

  // If Cluster is a standalone Cluster or a PRIMARY Cluster it cannot be fenced
  if (!is_cluster_set_member() ||
      (is_cluster_set_member() && !is_primary_cluster())) {
    return false;
  }

  if (!mysqlshdk::gr::is_active_member(*primary)) {
    return false;
  } else if (!mysqlshdk::gr::is_primary(*primary)) {
    // Try to get the primary member, if we cannot return false right away since
    // we cannot determine whether the cluster is fenced or not
    try {
      primary = get_primary_member_from_group(primary);
    } catch (...) {
      log_info(
          "Failed to get primary member from cluster '%s' using the session "
          "from '%s': %s",
          get_name().c_str(), primary->descr().c_str(),
          format_active_exception().c_str());
      return false;
    }
  }

  // Check if GR group action 'mysql_disable_super_read_only_if_primary' is
  // enabled
  bool disable_sro_if_primary_enabled = false;
  try {
    mysqlshdk::gr::get_member_action_status(
        *primary, mysqlshdk::gr::k_gr_disable_super_read_only_if_primary,
        &disable_sro_if_primary_enabled);
  } catch (...) {
    log_debug(
        "Failed to get status of GR member action "
        "'mysql_disable_super_read_only_if_primary' from '%s': %s",
        primary->descr().c_str(), format_active_exception().c_str());
    return false;
  }

  if (disable_sro_if_primary_enabled) {
    return false;
  }

  // Check if SRO is enabled on the primary member
  if (!primary->get_sysvar_bool("super_read_only", false)) {
    return false;
  }

  return true;
}

shcore::Value Cluster_impl::create_cluster_set(
    const std::string &domain_name,
    const clusterset::Create_cluster_set_options &options) {
  try {
    check_preconditions("createClusterSet");
  } catch (const shcore::Exception &e) {
    if (e.code() == SHERR_DBA_GROUP_HAS_NO_QUORUM) {
      // NOTE: This overrides the original quorum error, it was implemented this
      // way in Create_cluster_set operation
      current_console()->print_error(
          "Target cluster status is 'NO_QUORUM' which is not valid for "
          "InnoDB ClusterSet.");
      throw shcore::Exception("Invalid Cluster status: NO_QUORUM.",
                              SHERR_DBA_CLUSTER_STATUS_INVALID);
    } else {
      throw;
    }
  }

  // put an exclusive lock on the cluster
  auto c_lock = get_lock_exclusive();

  return Cluster_set_impl::create_cluster_set(this, domain_name, options);
}

std::shared_ptr<ClusterSet> Cluster_impl::get_cluster_set() {
  check_preconditions("getClusterSet");

  auto cs = get_cluster_set_object();

  // Check if the target Cluster is invalidated
  if (is_invalidated()) {
    mysqlsh::current_console()->print_warning(
        "Cluster '" + get_name() +
        "' was INVALIDATED and must be removed from the ClusterSet.");
  }

  // Check if the target Cluster still belongs to the ClusterSet
  Cluster_set_member_metadata cluster_member_md;
  if (!cs->get_metadata_storage()->get_cluster_set_member(get_id(),
                                                          &cluster_member_md)) {
    current_console()->print_error("The Cluster '" + get_name() +
                                   "' appears to have been removed from "
                                   "the ClusterSet '" +
                                   cs->get_name() +
                                   "', however its own metadata copy wasn't "
                                   "properly updated during the removal");

    throw shcore::Exception("The cluster '" + get_name() +
                                "' is not a part of the ClusterSet '" +
                                cs->get_name() + "'",
                            SHERR_DBA_CLUSTER_DOES_NOT_BELONG_TO_CLUSTERSET);
  }

  return std::make_shared<ClusterSet>(cs);
}

void Cluster_impl::check_cluster_online() {
  std::string role = is_primary_cluster() ? "Primary Cluster" : "Cluster";

  switch (cluster_availability()) {
    case Cluster_availability::ONLINE:
      break;
    case Cluster_availability::ONLINE_NO_PRIMARY:
      throw shcore::Exception(
          "Could not connect to PRIMARY of " + role + " '" + get_name() + "'",
          SHERR_DBA_CLUSTER_PRIMARY_UNAVAILABLE);

    case Cluster_availability::OFFLINE:
      throw shcore::Exception(
          "All members of " + role + " '" + get_name() + "' are OFFLINE",
          SHERR_DBA_GROUP_OFFLINE);

    case Cluster_availability::SOME_UNREACHABLE:
      throw shcore::Exception(
          "All reachable members of " + role + " '" + get_name() +
              "' are OFFLINE, but there are some unreachable members that "
              "could be ONLINE",
          SHERR_DBA_GROUP_OFFLINE);

    case Cluster_availability::NO_QUORUM:
      throw shcore::Exception("Could not connect to an ONLINE member of " +
                                  role + " '" + get_name() + "' within quorum",
                              SHERR_DBA_GROUP_HAS_NO_QUORUM);

    case Cluster_availability::UNREACHABLE:
      throw shcore::Exception("Could not connect to any ONLINE member of " +
                                  role + " '" + get_name() + "'",
                              SHERR_DBA_GROUP_UNREACHABLE);
  }
}

std::shared_ptr<Cluster_set_impl> Cluster_impl::get_cluster_set_object(
    bool print_warnings, bool check_status) const {
  if (m_cs_md_remove_pending) {
    throw shcore::Exception("Cluster is not part of a ClusterSet",
                            SHERR_DBA_CLUSTER_DOES_NOT_BELONG_TO_CLUSTERSET);
  }

  if (auto ptr = m_cluster_set.lock(); ptr) {
    if (check_status) ptr->get_primary_cluster()->check_cluster_online();
    return ptr;
  }

  // NOTE: The operation must work even if the PRIMARY cluster is not reachable,
  // as long as the target instance is a reachable member of an InnoDB Cluster
  // that is part of a ClusterSet.
  Cluster_metadata cluster_md = get_metadata();
  auto cluster_id = cluster_md.cluster_id;

  Cluster_set_metadata cset_md;
  auto metadata = get_metadata_storage();

  // Get the ClusterSet metadata handle
  if (!metadata->get_cluster_set(cluster_md.cluster_set_id, false, &cset_md,
                                 nullptr)) {
    throw shcore::Exception("No metadata found for the ClusterSet that " +
                                get_cluster_server()->descr() + " belongs to.",
                            SHERR_DBA_METADATA_MISSING);
  }

  auto cs = std::make_shared<Cluster_set_impl>(cset_md, get_cluster_server(),
                                               metadata);

  // Acquire primary
  cs->acquire_primary(true, check_status);

  try {
    // Verify the Primary Cluster's availability
    cs->get_primary_cluster()->check_cluster_online();
  } catch (const shcore::Exception &e) {
    if (check_status) throw;

    if (e.code() == SHERR_DBA_GROUP_HAS_NO_PRIMARY ||
        e.code() == SHERR_DBA_GROUP_REPLICATION_NOT_RUNNING) {
      log_warning("Could not connect to any member of the PRIMARY Cluster: %s",
                  e.what());
      if (print_warnings) {
        current_console()->print_warning(
            "Could not connect to any member of the PRIMARY Cluster, topology "
            "changes will not be allowed");
      }
    } else if (e.code() == SHERR_DBA_GROUP_HAS_NO_QUORUM) {
      log_warning("PRIMARY Cluster doesn't have quorum: %s", e.what());
      if (print_warnings) {
        auto console = current_console();
        console->print_warning(
            "The PRIMARY Cluster lost the quorum, topology changes will "
            "not be allowed");
        console->print_note(
            "To restore the Cluster and ClusterSet operations, restore the "
            "quorum on the PRIMARY Cluster using "
            "<<<forceQuorumUsingPartitionOf>>>()");
      }
    }
  }

  // Return a handle (Cluster_set_impl) to the ClusterSet the target Cluster
  // belongs to
  return cs;
}

void Cluster_impl::refresh_connections() {
  get_metadata_storage()->get_md_server()->reconnect_if_needed("Metadata");

  if (m_primary_master) {
    m_primary_master->reconnect_if_needed("Primary");
  }

  if (m_cluster_server) {
    m_cluster_server->reconnect_if_needed("Cluster");
  }
}

void Cluster_impl::setup_admin_account(const std::string &username,
                                       const std::string &host,
                                       const Setup_account_options &options) {
  check_preconditions("setupAdminAccount");

  // put a shared lock on the cluster
  auto c_lock = get_lock_shared();

  Base_cluster_impl::setup_admin_account(username, host, options);
}

void Cluster_impl::setup_router_account(const std::string &username,
                                        const std::string &host,
                                        const Setup_account_options &options) {
  check_preconditions("setupRouterAccount");

  // put a shared lock on the cluster
  auto c_lock = get_lock_shared();

  Base_cluster_impl::setup_router_account(username, host, options);
}

shcore::Value Cluster_impl::options(const bool all) {
  // Throw an error if the cluster has already been dissolved
  check_preconditions("options");

  // Create the Cluster_options command and execute it.
  cluster::Options op_option(*this, all);
  // Always execute finish when leaving "try catch".
  shcore::on_leave_scope finally([&op_option]() { op_option.finish(); });
  // Prepare the Cluster_options command execution (validations).
  op_option.prepare();

  // Execute Cluster_options operations.
  return op_option.execute();
}

void Cluster_impl::dissolve(const mysqlshdk::null_bool &force,
                            const bool interactive) {
  // We need to check if the group has quorum and if not we must abort the
  // operation otherwise GR blocks the writes to preserve the consistency
  // of the group and we end up with a hang.
  // This check is done at check_preconditions()
  try {
    check_preconditions("dissolve");
  } catch (const shcore::Exception &e) {
    // special case for dissolving a cluster that was removed from a clusterset
    // but its local metadata couldn't be updated
    if (e.code() == SHERR_DBA_CLUSTER_ALREADY_IN_CLUSTERSET &&
        m_cs_md_remove_pending) {
      log_info(
          "Dissolving cluster '%s' which was removed from a clusterset without "
          "a local metadata update.",
          get_name().c_str());
    } else {
      throw;
    }
  }

  // put an exclusive lock on the cluster
  auto c_lock = get_lock_exclusive();

  // Create the Dissolve command and execute it.
  cluster::Dissolve op_dissolve(interactive, force, this);

  // Always execute finish when leaving "try catch".
  shcore::on_leave_scope finally([&op_dissolve]() { op_dissolve.finish(); });

  // Prepare the dissolve command execution (validations).
  op_dissolve.prepare();
  // Execute dissolve operations.
  op_dissolve.execute();
}

void Cluster_impl::force_quorum_using_partition_of(
    const Connection_options &instance_def, const bool interactive) {
  std::shared_ptr<Instance> target_instance;
  std::string instance_address =
      instance_def.as_uri(mysqlshdk::db::uri::formats::only_transport());

  try {
    log_info("Opening a new session to the partition instance %s",
             instance_address.c_str());
    target_instance =
        Instance::connect(instance_def, current_shell_options()->get().wizards);
  }
  CATCH_REPORT_AND_THROW_CONNECTION_ERROR(instance_address)

  check_preconditions("forceQuorumUsingPartitionOf");

  // put an exclusive lock on the cluster
  auto c_lock = get_lock_exclusive();

  std::vector<Instance_metadata> online_instances = get_active_instances();

  if (online_instances.empty()) {
    throw shcore::Exception::logic_error(
        "No online instances are visible from the given one.");
  }

  auto group_peers_print =
      shcore::str_join(online_instances.begin(), online_instances.end(), ",",
                       [](const auto &i) { return i.endpoint; });

  // Remove the trailing comma of group_peers_print
  if (group_peers_print.back() == ',') group_peers_print.pop_back();

  if (interactive) {
    const auto console = current_console();

    std::string message = "Restoring cluster '" + get_name() +
                          "' from loss of quorum, by using the partition "
                          "composed of [" +
                          group_peers_print + "]\n";
    console->print_info(message);

    console->print_info("Restoring the InnoDB cluster ...");
    console->print_info();
  }

  // TODO(miguel): test if there's already quorum and add a 'force' option to
  // be used if so

  // TODO(miguel): test if the instance if part of the current cluster, for
  // the scenario of restoring a cluster quorum from another

  // Before rejoining an instance we must verify if the instance's
  // 'group_replication_group_name' matches the one registered in the
  // Metadata (BUG #26159339)
  {
    // Get instance address in metadata.
    std::string md_address = target_instance->get_canonical_address();

    // Check if the instance belongs to the Cluster on the Metadata
    if (!contains_instance_with_address(md_address)) {
      std::string message = "The instance '" + instance_address + "'";
      message.append(" does not belong to the cluster: '" + get_name() + "'.");
      throw shcore::Exception::runtime_error(message);
    }

    if (!validate_cluster_group_name(*target_instance, get_group_name())) {
      std::string nice_error =
          "The instance '" + instance_address +
          "' "
          "cannot be used to restore the cluster as it "
          "may belong to a different cluster as the one registered "
          "in the Metadata since the value of "
          "'group_replication_group_name' does not match the one "
          "registered in the cluster's Metadata: possible split-brain "
          "scenario.";

      throw shcore::Exception::runtime_error(nice_error);
    }
  }

  // Get the instance state
  Cluster_check_info state;

  auto instance_type = get_gr_instance_type(*target_instance);

  if (instance_type != TargetType::Standalone &&
      instance_type != TargetType::StandaloneWithMetadata &&
      instance_type != TargetType::StandaloneInMetadata) {
    state = get_replication_group_state(*target_instance, instance_type);

    if (state.source_state != ManagedInstance::OnlineRW &&
        state.source_state != ManagedInstance::OnlineRO) {
      std::string message = "The instance '" + instance_address + "'";
      message.append(" cannot be used to restore the cluster as it is on a ");
      message.append(ManagedInstance::describe(
          static_cast<ManagedInstance::State>(state.source_state)));
      message.append(" state, and should be ONLINE");

      throw shcore::Exception::runtime_error(message);
    }
  } else {
    std::string message = "The instance '" + instance_address + "'";
    message.append(
        " cannot be used to restore the cluster as it is not an active member "
        "of replication group.");

    throw shcore::Exception::runtime_error(message);
  }

  // Check if there is quorum to issue an error.
  if (mysqlshdk::gr::has_quorum(*target_instance, nullptr, nullptr)) {
    mysqlsh::current_console()->print_error(
        "Cannot perform operation on an healthy cluster because it can only "
        "be used to restore a cluster from quorum loss.");

    throw shcore::Exception::runtime_error(
        "The cluster has quorum according to instance '" + instance_address +
        "'");
  }

  // Get the all instances from MD and members visible by the target instance.
  std::vector<std::pair<Instance_metadata, mysqlshdk::gr::Member>>
      instances_info = get_instances_with_state();

  std::string group_peers;
  int online_count = 0;

  for (auto &instance : instances_info) {
    std::string instance_host = instance.first.endpoint;
    auto target_coptions = shcore::get_connection_options(instance_host, false);
    // We assume the login credentials are the same on all instances
    target_coptions.set_login_options_from(
        target_instance->get_connection_options());

    std::shared_ptr<Instance> instance_session;
    try {
      log_info(
          "Opening a new session to a group_peer instance to obtain the XCOM "
          "address %s",
          instance_host.c_str());
      instance_session = Instance::connect(
          target_coptions, current_shell_options()->get().wizards);

      if (instance.second.state == mysqlshdk::gr::Member_state::ONLINE ||
          instance.second.state == mysqlshdk::gr::Member_state::RECOVERING) {
        // Add GCS address of active instance to the force quorum membership.
        std::string group_peer_instance_xcom_address =
            instance_session
                ->get_sysvar_string("group_replication_local_address")
                .value_or("");

        group_peers.append(group_peer_instance_xcom_address);
        group_peers.append(",");

        online_count++;
      } else {
        // Stop GR on not active instances.
        mysqlshdk::gr::stop_group_replication(*instance_session);
      }
    } catch (const std::exception &e) {
      log_error("Could not open connection to %s: %s", instance_address.c_str(),
                e.what());

      // Only throw errors if the instance is active, otherwise ignore it.
      if (instance.second.state == mysqlshdk::gr::Member_state::ONLINE ||
          instance.second.state == mysqlshdk::gr::Member_state::RECOVERING) {
        throw;
      }
    }
  }

  if (online_count == 0) {
    throw shcore::Exception::logic_error(
        "No online instances are visible from the given one.");
  }

  // Force the reconfiguration of the GR group
  {
    // Remove the trailing comma of group_peers
    if (group_peers.back() == ',') group_peers.pop_back();

    log_info("Setting group_replication_force_members at instance %s",
             instance_address.c_str());

    // Setting the group_replication_force_members will force a new group
    // membership, triggering the necessary actions from GR upon being set to
    // force the quorum. Therefore, the variable can be cleared immediately
    // after it is set.
    target_instance->set_sysvar("group_replication_force_members", group_peers,
                                mysqlshdk::mysql::Var_qualifier::GLOBAL);

    // Clear group_replication_force_members at the end to allow GR to be
    // restarted later on the instance (without error).
    target_instance->set_sysvar("group_replication_force_members",
                                std::string(),
                                mysqlshdk::mysql::Var_qualifier::GLOBAL);
  }

  // If this is a REPLICA Cluster of a ClusterSet, ensure SRO is enabled on all
  // members
  if (is_cluster_set_member() && !is_primary_cluster()) {
    auto cs = get_cluster_set_object();
    cs->ensure_replica_settings(this, false);
  }

  if (interactive) {
    const auto console = current_console();

    console->print_info(
        "The InnoDB cluster was successfully restored using the partition "
        "from the instance '" +
        instance_def.as_uri(mysqlshdk::db::uri::formats::user_transport()) +
        "'.");
    console->print_info();
    console->print_info(
        "WARNING: To avoid a split-brain scenario, ensure that all other "
        "members of the cluster are removed or joined back to the group that "
        "was restored.");
    console->print_info();
  }
}

void Cluster_impl::rescan(const cluster::Rescan_options &options) {
  check_preconditions("rescan");

  // put an exclusive lock on the cluster
  auto c_lock = get_lock_exclusive();

  // Create the rescan command and execute it.
  cluster::Rescan op_rescan(options, this);

  // Always execute finish when leaving "try catch".
  shcore::on_leave_scope finally([&op_rescan]() { op_rescan.finish(); });

  // Prepare the rescan command execution (validations).
  op_rescan.prepare();

  // Execute rescan operation.
  op_rescan.execute();
}

bool Cluster_impl::get_disable_clone_option() const {
  shcore::Value value;
  if (m_metadata_storage->query_cluster_attribute(
          get_id(), k_cluster_attribute_disable_clone, &value)) {
    return value.as_bool();
  }
  return false;
}

void Cluster_impl::set_disable_clone_option(const bool disable_clone) {
  m_metadata_storage->update_cluster_attribute(
      get_id(), k_cluster_attribute_disable_clone,
      disable_clone ? shcore::Value::True() : shcore::Value::False());
}

bool Cluster_impl::get_manual_start_on_boot_option() const {
  shcore::Value flag;
  if (m_metadata_storage->query_cluster_attribute(
          get_id(), k_cluster_attribute_manual_start_on_boot, &flag))
    return flag.as_bool();
  // default false
  return false;
}

const std::string Cluster_impl::get_view_change_uuid() const {
  shcore::Value view_change_uuid;

  if (m_metadata_storage->query_cluster_attribute(
          get_id(), "group_replication_view_change_uuid", &view_change_uuid)) {
    return view_change_uuid.as_string();
  }

  // default empty
  return "";
}

shcore::Value Cluster_impl::check_instance_state(
    const Connection_options &instance_def) {
  check_preconditions("checkInstanceState");

  Scoped_instance target(connect_target_instance(instance_def, true));

  // Create the Cluster Check_instance_state object and execute it.
  cluster::Check_instance_state op_check_instance_state(*this, target);

  // Always execute finish when leaving "try catch".
  auto finally = shcore::on_leave_scope(
      [&op_check_instance_state]() { op_check_instance_state.finish(); });

  // Prepare the Cluster Check_instance_state  command execution
  // (validations).
  op_check_instance_state.prepare();

  // Execute Cluster Check_instance_state  operations.
  return op_check_instance_state.execute();
}

void Cluster_impl::switch_to_single_primary_mode(
    const Connection_options &instance_def) {
  check_preconditions("switchToSinglePrimaryMode");

  // Switch to single-primary mode

  // put an exclusive lock on the cluster
  auto c_lock = get_lock_exclusive();

  // Create the Switch_to_single_primary_mode object and execute it.
  cluster::Switch_to_single_primary_mode op_switch_to_single_primary_mode(
      instance_def, this);

  // Always execute finish when leaving "try catch".
  shcore::on_leave_scope finally([&op_switch_to_single_primary_mode]() {
    op_switch_to_single_primary_mode.finish();
  });

  // Prepare the Switch_to_single_primary_mode command execution
  // (validations).
  op_switch_to_single_primary_mode.prepare();

  // Execute Switch_to_single_primary_mode operation.
  op_switch_to_single_primary_mode.execute();
}

void Cluster_impl::switch_to_multi_primary_mode() {
  check_preconditions("switchToMultiPrimaryMode");

  // put an exclusive lock on the cluster
  auto c_lock = get_lock_exclusive();

  // Create the Switch_to_multi_primary_mode object and execute it.
  cluster::Switch_to_multi_primary_mode op_switch_to_multi_primary_mode(this);

  // Always execute finish when leaving "try catch".
  shcore::on_leave_scope finally([&op_switch_to_multi_primary_mode]() {
    op_switch_to_multi_primary_mode.finish();
  });

  // Prepare the Switch_to_multi_primary_mode command execution (validations).
  op_switch_to_multi_primary_mode.prepare();

  // Execute Switch_to_multi_primary_mode operation.
  op_switch_to_multi_primary_mode.execute();
}

void Cluster_impl::set_primary_instance(
    const Connection_options &instance_def,
    const cluster::Set_primary_instance_options &options) {
  check_preconditions("setPrimaryInstance");

  // Set primary instance

  // put an exclusive lock on the cluster
  auto c_lock = get_lock_exclusive();

  // Create the Set_primary_instance object and execute it.
  cluster::Set_primary_instance op_set_primary_instance(instance_def, this,
                                                        options);

  // Always execute finish when leaving "try catch".
  shcore::on_leave_scope finally(
      [&op_set_primary_instance]() { op_set_primary_instance.finish(); });

  // Prepare the Set_primary_instance command execution (validations).
  op_set_primary_instance.prepare();

  // Execute Set_primary_instance operation.
  op_set_primary_instance.execute();
}

mysqlshdk::utils::Version Cluster_impl::get_lowest_instance_version(
    std::vector<std::string> *out_instances_addresses) const {
  std::string current_instance_address;
  // Get the server version of the current cluster instance and initialize
  // the lowest cluster session.
  mysqlshdk::utils::Version lowest_version =
      get_cluster_server()->get_version();

  // Get address of the cluster instance to skip it in the next step.
  std::string cluster_instance_address =
      get_cluster_server()->get_canonical_address();

  // Get the lowest server version from the available cluster instances.
  execute_in_members({mysqlshdk::gr::Member_state::ONLINE,
                      mysqlshdk::gr::Member_state::RECOVERING},
                     get_cluster_server()->get_connection_options(),
                     {cluster_instance_address},
                     [&lowest_version, &out_instances_addresses](
                         const std::shared_ptr<Instance> &instance,
                         const mysqlshdk::gr::Member &) {
                       mysqlshdk::utils::Version version =
                           instance->get_version();

                       if (version < lowest_version) {
                         lowest_version = version;

                         if (out_instances_addresses) {
                           out_instances_addresses->push_back(
                               instance->get_canonical_address());
                         }
                       }
                       return true;
                     });

  return lowest_version;
}

std::pair<mysqlshdk::mysql::Auth_options, std::string>
Cluster_impl::create_replication_user(mysqlshdk::mysql::IInstance *target,
                                      std::string_view auth_cert_subject,
                                      bool only_on_target,
                                      mysqlshdk::mysql::Auth_options creds,
                                      bool print_recreate_note) const {
  assert(target);

  auto primary = target;

  auto host = get_replication_user_host();

  if (creds.user.empty()) {
    creds.user = make_replication_user_name(
        target->get_server_id(), mysqlshdk::gr::k_group_recovery_user_prefix);
  }

  std::shared_ptr<mysqlshdk::mysql::IInstance> primary_master;
  if (!only_on_target) {
    primary_master = get_primary_master();
    primary = primary_master.get();
  }

  log_info("Creating recovery account '%s'@'%s' for instance '%s'",
           creds.user.c_str(), host.c_str(), target->descr().c_str());

  // Get all hosts for the recovery account:
  //
  // There may be left-over accounts in the target instance that must be
  // cleared-out, especially because when using the MySQL Communication Stack a
  // new account is created on both ends (joiner and donor) to allow GCS
  // establish a connection before joining the member to the group and if the
  // account already exists with a different host that is reachable, GCS will
  // fail to establish a connection and the member won't be able to join the
  // group. For that reason, we must ensure all of those recovery accounts are
  // dropped and not only the one of the 'replicationAllowedHost'
  std::vector<std::string> recovery_user_hosts =
      mysqlshdk::mysql::get_all_hostnames_for_user(*primary, creds.user);

  // Check if the replication user already exists and delete it if it does,
  // before creating it again.
  for (const auto &hostname : recovery_user_hosts) {
    if (!primary->user_exists(creds.user, hostname)) continue;

    auto note = shcore::str_format(
        "User '%s'@'%s' already existed at instance '%s'. It will be "
        "deleted and created again with a new password.",
        creds.user.c_str(), hostname.c_str(), primary->descr().c_str());

    if (print_recreate_note) {
      current_console()->print_note(note);
    } else {
      log_debug("%s", note.c_str());
    }

    primary->drop_user(creds.user, hostname);
  }

  // Check if clone is available on ALL cluster members, to avoid a failing
  // SQL query because BACKUP_ADMIN is not supported
  // Do the same for MySQL commStack, being the grant GROUP_REPLICATION_STREAM
  mysqlshdk::utils::Version lowest_version = get_lowest_instance_version();
  bool clone_available_all_members = supports_mysql_clone(lowest_version);
  bool mysql_comm_stack_available_all_members =
      supports_mysql_communication_stack(lowest_version);

  auto target_version = target->get_version();

  std::vector<std::string> hosts;
  hosts.push_back(host);

  mysqlshdk::gr::Create_recovery_user_options user_options;
  user_options.clone_supported =
      clone_available_all_members && supports_mysql_clone(target_version);
  user_options.auto_failover = false;
  user_options.mysql_comm_stack_supported =
      mysql_comm_stack_available_all_members &&
      supports_mysql_communication_stack(target_version);

  // setup password, cert issuer and/or subject
  auto auth_type = query_cluster_auth_type();
  validate_instance_member_auth_options("cluster", false, auth_type,
                                        auth_cert_subject);

  switch (auth_type) {
    case Replication_auth_type::PASSWORD:
    case Replication_auth_type::CERT_ISSUER_PASSWORD:
    case Replication_auth_type::CERT_SUBJECT_PASSWORD:
      user_options.requires_password = true;
      user_options.password = creds.password;
      break;
    default:
      user_options.requires_password = false;
      break;
  }

  switch (auth_type) {
    case Replication_auth_type::CERT_SUBJECT:
    case Replication_auth_type::CERT_SUBJECT_PASSWORD:
      user_options.cert_subject = auth_cert_subject;
      [[fallthrough]];
    case Replication_auth_type::CERT_ISSUER:
    case Replication_auth_type::CERT_ISSUER_PASSWORD:
      user_options.cert_issuer = query_cluster_auth_cert_issuer();
      break;
    default:
      break;
  }

  return {mysqlshdk::gr::create_recovery_user(creds.user, *primary, hosts,
                                              user_options),
          host};
}

void Cluster_impl::reset_recovery_password(
    const std::shared_ptr<Instance> &target,
    const std::string &recovery_account,
    const std::vector<std::string> &hosts) const {
  bool needs_password{false};
  switch (auto auth_type = query_cluster_auth_type(); auth_type) {
    case Replication_auth_type::PASSWORD:
    case Replication_auth_type::CERT_ISSUER_PASSWORD:
    case Replication_auth_type::CERT_SUBJECT_PASSWORD:
      needs_password = true;
    default:
      break;
  }

  // Get the recovery account for the target instance
  std::string recovery_user = recovery_account;
  std::vector<std::string> recovery_user_hosts = hosts;

  if (recovery_user.empty() && recovery_user_hosts.empty()) {
    std::tie(recovery_user, recovery_user_hosts, std::ignore) =
        get_replication_user(*target);
  }

  log_info("Resetting recovery account credentials for '%s'%s.",
           target->descr().c_str(),
           needs_password ? " (with password)" : " (without password)");

  // Generate and set a new password and update the replication credentials
  mysqlshdk::mysql::Replication_credentials_options options;
  if (needs_password) {
    std::string password;
    mysqlshdk::mysql::set_random_password(*get_primary_master(), recovery_user,
                                          recovery_user_hosts, &password);

    options.password = std::move(password);
  }

  mysqlshdk::mysql::change_replication_credentials(
      *target, mysqlshdk::gr::k_gr_recovery_channel, recovery_user, options);
}

std::pair<std::string, std::string> Cluster_impl::recreate_replication_user(
    const std::shared_ptr<Instance> &target,
    std::string_view auth_cert_subject) const {
  auto [repl_account, repl_account_host] = create_replication_user(
      target.get(), auth_cert_subject, false, {}, false);

  mysqlshdk::mysql::Replication_credentials_options options;
  options.password = repl_account.password.value_or("");

  mysqlshdk::mysql::change_replication_credentials(
      *target, mysqlshdk::gr::k_gr_recovery_channel, repl_account.user,
      options);

  return std::pair<std::string, std::string>(repl_account.user,
                                             repl_account_host);
}

bool Cluster_impl::drop_replication_user(
    mysqlshdk::mysql::IInstance *target, const std::string &endpoint,
    const std::string &server_uuid, const uint32_t server_id,
    mysqlshdk::mysql::Sql_undo_list *undo) {
  // Either target is a valid pointer or endpoint & server_uuid are set
  assert(target || (!endpoint.empty() && !server_uuid.empty()));

  auto console = current_console();
  auto primary = get_primary_master();
  std::string recovery_user, recovery_user_host;
  std::vector<std::string> recovery_user_hosts;

  std::string target_endpoint = endpoint;
  std::string target_server_uuid = server_uuid;
  uint32_t target_server_id = server_id;
  bool from_metadata = false;

  // Get the endpoint, uuid and server_id if empty
  if (target) {
    if (target_endpoint.empty()) {
      target_endpoint = target->get_canonical_address();
    }
    if (target_server_uuid.empty()) {
      target_server_uuid = target->get_uuid();
    }
    if (target_server_id == 0) {
      target_server_id = target->get_id();
    }
  }

  // Check if the recovery account created for the instance when it
  // joined the cluster, is in use by any other member. If not, it is a leftover
  // that must be dropped. This scenario happens when an instance is added to a
  // cluster using clone as the recovery method but waitRecovery is zero or the
  // user cancelled the monitoring of the recovery so the instance will use the
  // seed's recovery account leaving its own account unused.
  //
  // TODO(anyone): BUG#30031815 requires that we skip if the primary instance is
  // being removed. It causes a primary election and we cannot know which
  // instance will be the primary to perform the cleanup
  if (!mysqlshdk::utils::are_endpoints_equal(
          target_endpoint, primary->get_canonical_address())) {
    // Generate the recovery account username that was created for the
    // instance
    std::string user = make_replication_user_name(
        target_server_id, mysqlshdk::gr::k_group_recovery_user_prefix);

    // Get the Cluster's allowedHost
    std::string host = "%";

    if (shcore::Value allowed_host;
        get_metadata_storage()->query_cluster_attribute(
            get_id(), k_cluster_attribute_replication_allowed_host,
            &allowed_host) &&
        allowed_host.type == shcore::String &&
        !allowed_host.as_string().empty()) {
      host = allowed_host.as_string();
    }

    // The account must not be in use by any other member of the cluster
    if (get_metadata_storage()->count_recovery_account_uses(user) == 0) {
      log_info("Dropping recovery account '%s'@'%s' for instance '%s'",
               user.c_str(), host.c_str(), target_endpoint.c_str());

      try {
        if (undo) {
          undo->add_snapshot_for_drop_user(*get_primary_master(), user, host);
        }
        get_primary_master()->drop_user(user, host, true);
      } catch (...) {
        mysqlsh::current_console()->print_warning(shcore::str_format(
            "Error dropping recovery account '%s'@'%s' for instance '%s': %s",
            user.c_str(), host.c_str(), target_endpoint.c_str(),
            format_active_exception().c_str()));
      }
    } else {
      log_info(
          "Recovery account '%s'@'%s' for instance '%s' still in use in "
          "the Cluster: Skipping its removal",
          user.c_str(), host.c_str(), target_endpoint.c_str());
    }
  }

  if (target) {
    // Attempt to get the recovery account
    try {
      std::tie(recovery_user, recovery_user_hosts, from_metadata) =
          get_replication_user(*target);
    } catch (const std::exception &e) {
      console->print_note(
          "The recovery user name for instance '" + target->descr() +
          "' does not match the expected format for users "
          "created automatically by InnoDB Cluster. Skipping its removal.");
    }

    if (from_metadata) {
      recovery_user_host = recovery_user_hosts.front();
    }
  } else {
    // Get the user from the Metadata
    std::tie(recovery_user, recovery_user_host) =
        get_metadata_storage()->get_instance_repl_account(
            target_server_uuid, Cluster_type::GROUP_REPLICATION);

    if (recovery_user.empty()) {
      log_warning("Could not obtain recovery account metadata for '%s'",
                  target_endpoint.c_str());
      return false;
    } else {
      from_metadata = true;
    }
  }

  // Drop the recovery account
  if (!from_metadata) {
    log_info("Removing replication user '%s'", recovery_user.c_str());
    try {
      auto hosts =
          mysqlshdk::mysql::get_all_hostnames_for_user(*primary, recovery_user);

      for (const auto &host : hosts) {
        if (undo) {
          undo->add_snapshot_for_drop_user(*primary, recovery_user, host);
        }
        primary->drop_user(recovery_user, host, true);
      }
    } catch (const std::exception &e) {
      console->print_warning("Error dropping recovery accounts for user " +
                             recovery_user + ": " + e.what());
      return false;
    }
  } else {
    /*
    Since clone copies all the data, including mysql.slave_master_info
    (where the CHANGE MASTER credentials are stored) an instance may be
    using the info stored in the donor's mysql.slave_master_info.

    To avoid such situation, we re-issue the CHANGE MASTER query after
    clone to ensure the right account is used. However, if the monitoring
    process is interrupted or waitRecovery = 0, that won't be done.

    The approach to tackle this issue is to store the donor recovery account
    in the target instance MD.instances table before doing the new CHANGE
    and only update it to the right account after a successful CHANGE
    MASTER. With this approach we can ensure that the account is not removed
    if it is being used.

    As so were we must query the Metadata to check whether the
    recovery user of that instance is registered for more than one instance
    and if that's the case then it won't be dropped.
    */
    if (get_metadata_storage()->count_recovery_account_uses(recovery_user) ==
        1) {
      log_info("Dropping recovery account '%s'@'%s' for instance '%s'",
               recovery_user.c_str(), recovery_user_host.c_str(),
               target_endpoint.c_str());

      try {
        if (undo) {
          undo->add_snapshot_for_drop_user(*primary, recovery_user,
                                           recovery_user_host);
        }
        primary->drop_user(recovery_user, recovery_user_host, true);
      } catch (...) {
        console->print_warning(shcore::str_format(
            "Error dropping recovery account '%s'@'%s' for instance '%s': %s",
            recovery_user.c_str(), recovery_user_host.c_str(),
            target_endpoint.c_str(), format_active_exception().c_str()));
      }

      // Also update metadata
      try {
        std::unique_ptr<Transaction_undo> trx_undo = Transaction_undo::create();

        get_metadata_storage()->update_instance_repl_account(
            target_server_uuid, Cluster_type::GROUP_REPLICATION, "", "",
            trx_undo.get());

        if (undo) undo->add(std::move(trx_undo));
      } catch (const std::exception &e) {
        log_warning("Could not update recovery account metadata for '%s': %s",
                    target_endpoint.c_str(), e.what());
      }
    }
  }

  return true;
}

void Cluster_impl::drop_replication_users(
    mysqlshdk::mysql::Sql_undo_list *undo) {
  auto ipool = current_ipool();

  execute_in_members(
      [this, undo](const std::shared_ptr<Instance> &instance,
                   const Instance_md_and_gr_member &info) {
        try {
          drop_replication_user(instance.get(), "", "", 0, undo);
        } catch (...) {
          current_console()->print_warning("Could not drop internal user for " +
                                           info.first.endpoint + ": " +
                                           format_active_exception());
        }
        return true;
      },
      [this, undo](const shcore::Error &connect_error,
                   const Instance_md_and_gr_member &info) {
        // drop user based on MD lookup (will not work if the instance is still
        // using legacy account management)
        if (!drop_replication_user(nullptr, info.first.endpoint,
                                   info.first.uuid, info.first.server_id,
                                   undo)) {
          current_console()->print_warning("Could not drop internal user for " +
                                           info.first.endpoint + ": " +
                                           connect_error.format());
        }
        return true;
      });
}

void Cluster_impl::wipe_all_replication_users() {
  auto primary = get_primary_master();

  // Get all recovery and clusterset replication accounts
  std::vector<std::string> accounts_to_drop;

  std::string rec_prefix =
      std::string(mysqlshdk::gr::k_group_recovery_user_prefix) + "%";
  std::string cs_prefix = std::string(k_cluster_set_async_user_name) + "%";

  auto result = primary->queryf(
      "SELECT user from mysql.user WHERE user LIKE ? OR user LIKE ?",
      rec_prefix, cs_prefix);
  while (auto row = result->fetch_one()) {
    accounts_to_drop.push_back(row->get_string(0));
  }

  // Drop all accounts
  for (const auto &account : accounts_to_drop) {
    log_info("Removing account '%s'", account.c_str());
    try {
      mysqlshdk::mysql::drop_all_accounts_for_user(*primary, account);
    } catch (const std::exception &e) {
      mysqlsh::current_console()->print_warning(
          "Error dropping accounts for user " + account + ": " + e.what());
    }
  }
}

bool Cluster_impl::contains_instance_with_address(
    const std::string &host_port) const {
  return get_metadata_storage()->is_instance_on_cluster(get_id(), host_port);
}

/*
 * Ensures the cluster object (and metadata) can perform update operations and
 * returns a session to the PRIMARY.
 *
 * For a cluster to be updatable, it's necessary that:
 * - the MD object is connected to the PRIMARY of the
 * primary cluster, so that the MD can be updated (and is also not lagged)
 * - the primary of the cluster is reachable, so that cluster accounts
 * can be created there.
 *
 * An exception is thrown if not possible to connect to the primary.
 *
 * An Instance object connected to the primary is returned. The session
 * is owned by the cluster object.
 */
mysqlsh::dba::Instance *Cluster_impl::acquire_primary(
    bool primary_required, bool check_primary_status) {
  if (!m_cluster_server) {
    return nullptr;
  }

  // clear cached clusterset related metadata
  m_cs_md.cluster_set_id.clear();

  std::string uuid = m_cluster_server->get_uuid();
  std::string primary_url;

  // Get the Metadata state and return right away if the Metadata is
  // nonexisting, upgrading, or the setup failed
  //
  // TODO(anyone) In such scenarios, there will be a double-check for the
  // Metadata state. This should be refactored.
  metadata::State md_state = m_metadata_storage->state();

  if (md_state == metadata::State::FAILED_SETUP ||
      md_state == metadata::State::FAILED_UPGRADE ||
      md_state == metadata::State::NONEXISTING ||
      md_state == metadata::State::UPGRADING) {
    return m_cluster_server.get();
  }

  auto console = current_console();

  // Ensure m_cluster_server points to the PRIMARY of the group
  try {
    primary_url = find_primary_member_uri(m_cluster_server, false, nullptr);

    if (primary_url.empty()) {
      // Might happen and it should be possible to check the Cluster's status in
      // such situation
      if (primary_required)
        current_console()->print_info("No PRIMARY member found for cluster '" +
                                      get_name() + "'");
    } else if (!mysqlshdk::utils::are_endpoints_equal(
                   primary_url,
                   m_cluster_server->get_connection_options().uri_endpoint())) {
      mysqlshdk::db::Connection_options copts(primary_url);
      log_info("Connecting to %s...", primary_url.c_str());
      copts.set_login_options_from(m_cluster_server->get_connection_options());

      auto new_primary = Instance::connect(copts);

      // Check if GR is active or not. If not active or in error state, throw an
      // exception and leave the attributes unchanged.
      // This may happen when a failover is happening at the moment
      auto instance_state = mysqlshdk::gr::get_member_state(*new_primary);

      if (instance_state == mysqlshdk::gr::Member_state::OFFLINE ||
          instance_state == mysqlshdk::gr::Member_state::ERROR) {
        primary_url.clear();
        throw shcore::Exception("PRIMARY instance is not ONLINE",
                                SHERR_DBA_GROUP_MEMBER_NOT_ONLINE);
      } else {
        m_cluster_server = new_primary;
        m_primary_master = m_cluster_server;
      }
    } else {
      if (!m_primary_master) {
        m_primary_master = m_cluster_server;
      }
    }
  } catch (...) {
    if (!primary_url.empty() && primary_required) {
      current_console()->print_error(
          "A connection to the PRIMARY instance at " + primary_url +
          " could not be established to perform this action.");
    }

    throw;
  }

  // Check if the Cluster belongs to a ClusterSet
  if (is_cluster_set_member()) {
    invalidate_cluster_set_metadata_cache();

    auto cs = get_cluster_set_object(true, check_primary_status);

    if (auto cs_primary_master = cs->get_primary_master()) {
      // Check if the ClusterSet Primary is different than the Cluster's one
      auto cs_primary_master_url =
          cs_primary_master->get_connection_options().uri_endpoint();

      // The ClusterSet Primary is different, establish a connection to it to
      // set it as the Cluster's primary
      if (!m_primary_master ||
          m_primary_master->get_connection_options().uri_endpoint() !=
              cs_primary_master_url) {
        mysqlshdk::db::Connection_options copts(cs_primary_master_url);
        log_info("Connecting to %s...", cs_primary_master_url.c_str());
        copts.set_login_options_from(
            cs_primary_master->get_connection_options());

        m_primary_master = Instance::connect(copts);
        m_metadata_storage =
            std::make_shared<MetadataStorage>(Instance::connect(copts));
      }

      // if we think we're the primary cluster, try connecting to replica
      // clusters and look for a newer view_id generation, in case we got
      // failed over from and invalidated
      if (is_primary_cluster()) {
        find_real_cluster_set_primary(cs.get());

        auto pc = cs->get_primary_cluster();
        // If the real PC is different, it means this Cluster is INVALIDATED

        if (pc->get_id() != get_id()) {
          // Set m_primary_master to the actual Primary of the Primary Cluster
          m_primary_master = pc->get_primary_master();
        }
      }
    } else {
      // If there's no global primary master, the PRIMARY Cluster is down
      m_primary_master = nullptr;
    }
  } else {
    log_debug("Cluster does not belong to a ClusterSet");
  }

  if (m_primary_master && m_metadata_storage->get_md_server()->get_uuid() !=
                              m_primary_master->get_uuid()) {
    m_metadata_storage = std::make_shared<MetadataStorage>(
        Instance::connect(m_primary_master->get_connection_options()));

    if (is_cluster_set_member()) {
      // Update the Cluster's Cluster_set_member_metadata data according to the
      // Correct Metadata
      // Note: This also marks the cluster as invalidated. That info might be
      // required later on
      m_metadata_storage->get_cluster_set_member(get_id(), &m_cs_md);
    }
  }

  return m_primary_master.get();
}

Cluster_metadata Cluster_impl::get_metadata() const {
  Cluster_metadata cmd;
  if (!get_metadata_storage()->get_cluster(get_id(), &cmd)) {
    throw shcore::Exception(
        "Cluster metadata could not be loaded for " + get_name(),
        SHERR_DBA_METADATA_MISSING);
  }
  return cmd;
}

std::shared_ptr<Cluster_set_impl>
Cluster_impl::check_and_get_cluster_set_for_cluster() {
  auto cs = get_cluster_set_object(true);

  current_ipool()->set_metadata(cs->get_metadata_storage());

  if (is_invalidated()) {
    current_console()->print_warning(
        "Cluster '" + get_name() +
        "' was INVALIDATED and must be removed from the ClusterSet.");
    return cs;
  }

  bool pc_changed = false;
  // if we think we're the primary cluster, try connecting to replica
  // clusters and look for a newer view_id generation, in case we got
  // failed over from and invalidated
  if (is_primary_cluster()) {
    find_real_cluster_set_primary(cs.get());

    auto pc = cs->get_primary_cluster();

    // If the real PC is different, it means we're actually invalidated
    pc_changed = pc->get_id() != get_id();
  }
  if (pc_changed || cs->get_metadata_storage()->get_md_server()->descr() !=
                        get_metadata_storage()->get_md_server()->descr()) {
    // Check if the cluster was removed from the cs according to the MD in
    // the correct primary cluster
    if (!cs->get_metadata_storage()->check_cluster_set(
            get_cluster_server().get())) {
      std::string msg =
          "The Cluster '" + get_name() +
          "' appears to have been removed from the ClusterSet '" +
          cs->get_name() +
          "', however its own metadata copy wasn't properly updated "
          "during the removal";
      current_console()->print_warning(msg);

      // Mark that the cluster was already removed from the clusterset but
      // doesn't know about it yet
      set_cluster_set_remove_pending(true);

      return nullptr;
    } else if (is_invalidated()) {
      current_console()->print_warning(
          "Cluster '" + get_name() +
          "' was INVALIDATED and must be removed from the ClusterSet.");
    }
  }

  return cs;
}

void Cluster_impl::release_primary() { m_primary_master.reset(); }

std::tuple<std::string, std::vector<std::string>, bool>
Cluster_impl::get_replication_user(
    const mysqlshdk::mysql::IInstance &target_instance) const {
  // First check the metadata for the recovery user
  std::string recovery_user, recovery_user_host;
  bool from_metadata = false;
  std::vector<std::string> recovery_user_hosts;
  std::tie(recovery_user, recovery_user_host) =
      get_metadata_storage()->get_instance_repl_account(
          target_instance.get_uuid(), Cluster_type::GROUP_REPLICATION);
  if (recovery_user.empty()) {
    // Assuming the account was created by an older version of the shell,
    // which did not store account name in metadata
    log_info(
        "No recovery account details in metadata for instance '%s', assuming "
        "old account style",
        target_instance.descr().c_str());

    auto unrecorded_recovery_user = mysqlshdk::mysql::get_replication_user(
        target_instance, mysqlshdk::gr::k_gr_recovery_channel);
    assert(!unrecorded_recovery_user.empty());

    if (shcore::str_beginswith(
            unrecorded_recovery_user,
            mysqlshdk::gr::k_group_recovery_old_user_prefix)) {
      log_info("Found old account style recovery user '%s'",
               unrecorded_recovery_user.c_str());
      // old accounts were created for several hostnames
      recovery_user_hosts = mysqlshdk::mysql::get_all_hostnames_for_user(
          *(get_cluster_server()), unrecorded_recovery_user);
      recovery_user = unrecorded_recovery_user;
    } else if (shcore::str_beginswith(
                   unrecorded_recovery_user,
                   mysqlshdk::gr::k_group_recovery_user_prefix)) {
      // either the transaction to store the user failed or the metadata was
      // manually changed to remove it.
      throw shcore::Exception::metadata_error(
          "The replication recovery account in use by '" +
          target_instance.descr() +
          "' is not stored in the metadata. Use cluster.rescan() to update "
          "the metadata.");
    } else {
      // account not created by InnoDB cluster
      throw shcore::Exception::runtime_error(
          shcore::str_format("Recovery user '%s' not created by InnoDB Cluster",
                             unrecorded_recovery_user.c_str()));
    }
  } else {
    from_metadata = true;
    // new recovery user format, stored in Metadata.
    recovery_user_hosts.push_back(recovery_user_host);
  }
  return std::make_tuple(recovery_user, recovery_user_hosts, from_metadata);
}

std::unordered_map<uint32_t, std::string>
Cluster_impl::get_mismatched_recovery_accounts() const {
  std::unordered_map<uint32_t, std::string> accounts;

  // don't run the test if the metadata needs an upgrade
  if (m_metadata_storage->installed_version() !=
      mysqlsh::dba::metadata::current_version())
    return accounts;

  m_metadata_storage->iterate_recovery_account(
      [&accounts](uint32_t server_id, std::string recovery_account) {
        // must match either the new prefix or the old one
        auto account = shcore::str_format(
            "%s%u", mysqlshdk::gr::k_group_recovery_user_prefix, server_id);
        if (account == recovery_account) return true;

        account = shcore::str_format(
            "%s%u", mysqlshdk::gr::k_group_recovery_old_user_prefix, server_id);
        if (account == recovery_account) return true;

        accounts[server_id] = std::move(recovery_account);
        return true;
      });

  return accounts;
}

std::vector<std::tuple<std::string, std::string>>
Cluster_impl::get_unused_recovery_accounts(
    const std::unordered_map<uint32_t, std::string>
        &mismatched_recovery_accounts) const {
  // don't run the test if the metadata needs an upgrade
  if (m_metadata_storage->installed_version() !=
      mysqlsh::dba::metadata::current_version())
    return {};

  auto recovery_users = m_metadata_storage->get_recovery_account_users();

  std::vector<std::tuple<std::string, std::string>> accounts;
  mysqlshdk::mysql::iterate_users(
      *m_cluster_server, "mysql\\_innodb\\_cluster\\_%",
      [&accounts, &mismatched_recovery_accounts, &recovery_users](
          std::string user, std::string host) {
        if (std::find(recovery_users.begin(), recovery_users.end(), user) !=
            recovery_users.end())
          return true;

        auto server_id = extract_server_id(user);
        if (server_id < 0) return true;

        if (mismatched_recovery_accounts.find(server_id) ==
            mismatched_recovery_accounts.end()) {
          accounts.push_back({std::move(user), std::move(host)});
        }

        return true;
      });

  return accounts;
}

std::shared_ptr<Instance> Cluster_impl::get_session_to_cluster_instance(
    const std::string &instance_address) const {
  // Set login credentials to connect to instance.
  // use the host and port from the instance address
  Connection_options instance_cnx_opts =
      shcore::get_connection_options(instance_address, false);
  // Use the password from the cluster session.
  Connection_options cluster_cnx_opt =
      get_cluster_server()->get_connection_options();
  instance_cnx_opts.set_login_options_from(cluster_cnx_opt);

  log_debug("Connecting to instance '%s'", instance_address.c_str());
  try {
    // Try to connect to instance
    auto instance = Instance::connect(instance_cnx_opts);
    log_debug("Successfully connected to instance");
    return instance;
  } catch (const std::exception &err) {
    log_debug("Failed to connect to instance: %s", err.what());
    throw;
  }
}

size_t Cluster_impl::setup_clone_plugin(bool enable_clone) const {
  // Get all cluster instances
  std::vector<Instance_metadata> instance_defs = get_instances();

  // Counter for instances that failed to be updated
  size_t count = 0;

  auto ipool = current_ipool();

  for (const Instance_metadata &instance_def : instance_defs) {
    std::string instance_address = instance_def.endpoint;

    // Establish a session to the cluster instance
    try {
      Scoped_instance instance(
          ipool->connect_unchecked_endpoint(instance_address));

      // Handle the plugin setup only if the target instance supports it
      if (supports_mysql_clone(instance->get_version())) {
        if (!enable_clone) {
          // Uninstall the clone plugin
          log_info("Uninstalling the clone plugin on instance '%s'.",
                   instance_address.c_str());
          mysqlshdk::mysql::uninstall_clone_plugin(*instance, nullptr);
        } else {
          // Install the clone plugin
          log_info("Installing the clone plugin on instance '%s'.",
                   instance_address.c_str());
          mysqlshdk::mysql::install_clone_plugin(*instance, nullptr);

          // Get the recovery account in use by the instance so the grant
          // BACKUP_ADMIN is granted to its recovery account
          {
            std::string recovery_user;
            std::vector<std::string> recovery_user_hosts;
            try {
              std::tie(recovery_user, recovery_user_hosts, std::ignore) =
                  get_replication_user(*instance);
            } catch (const shcore::Exception &re) {
              if (re.is_runtime()) {
                mysqlsh::current_console()->print_error(
                    "Unsupported recovery account has been found for "
                    "instance " +
                    instance->descr() +
                    ". Operations such as "
                    "<Cluster>.<<<resetRecoveryAccountsPassword>>>() and "
                    "<Cluster>.<<<addInstance>>>() may fail. Please remove and "
                    "add the instance back to the Cluster to ensure a "
                    "supported recovery account is used.");
              }
              throw;
            }

            auto primary = get_primary_master();

            // Add the BACKUP_ADMIN grant to the instance's recovery account
            // since it may not be there if the instance was added to a
            // cluster non-supporting clone
            for (const auto &host : recovery_user_hosts) {
              shcore::sqlstring grant("GRANT BACKUP_ADMIN ON *.* TO ?@?", 0);
              grant << recovery_user;
              grant << host;
              grant.done();
              primary->execute(grant);
            }
          }
        }
      }
    } catch (const shcore::Error &err) {
      auto console = mysqlsh::current_console();

      std::string op = enable_clone ? "enable" : "disable";

      std::string err_msg = "Unable to " + op + " clone on the instance '" +
                            instance_address + "': " + err.format();

      // If a cluster member is unreachable, just print a warning. Otherwise
      // print error
      if (err.code() == CR_CONNECTION_ERROR ||
          err.code() == CR_CONN_HOST_ERROR) {
        console->print_warning(err_msg);
      } else {
        console->print_error(err_msg);
      }
      console->print_info();

      // It failed to update this instance, so increment the counter
      count++;
    }
  }

  return count;
}

void Cluster_impl::_set_instance_option(const std::string &instance_def,
                                        const std::string &option,
                                        const shcore::Value &value) {
  auto instance_conn_opt = Connection_options(instance_def);

  // Set Cluster configuration option

  // Create the Cluster Set_instance_option object and execute it.
  std::unique_ptr<cluster::Set_instance_option> op_set_instance_option;

  // Validation types due to a limitation on the expose() framework.
  // Currently, it's not possible to do overloading of functions that overload
  // an argument of type string/int since the type int is convertible to
  // string, thus overloading becomes ambiguous. As soon as that limitation is
  // gone, this type checking shall go away too.
  if (value.type == shcore::String) {
    std::string value_str = value.as_string();
    op_set_instance_option = std::make_unique<cluster::Set_instance_option>(
        *this, instance_conn_opt, option, value_str);
  } else if (value.type == shcore::Integer || value.type == shcore::UInteger) {
    int64_t value_int = value.as_int();
    op_set_instance_option = std::make_unique<cluster::Set_instance_option>(
        *this, instance_conn_opt, option, value_int);
  } else {
    throw shcore::Exception::type_error(
        "Argument #3 is expected to be a string or an integer");
  }

  // Always execute finish when leaving "try catch".
  shcore::on_leave_scope finally(
      [&op_set_instance_option]() { op_set_instance_option->finish(); });

  // Prepare the Cluster Set_instance_option command execution (validations).
  op_set_instance_option->prepare();

  // put an exclusive lock on the target instance
  mysqlshdk::mysql::Lock_scoped i_lock;
  if (option != kLabel) {
    if (auto instance = op_set_instance_option->get_target_instance(); instance)
      i_lock = instance->get_lock_exclusive();
  }

  // Execute Cluster Set_instance_option operations.
  op_set_instance_option->execute();
}

void Cluster_impl::_set_option(const std::string &option,
                               const shcore::Value &value) {
  // put an exclusive lock on the cluster
  mysqlshdk::mysql::Lock_scoped c_lock;
  if (option != kClusterName) c_lock = get_lock_exclusive();

  // Set Cluster configuration option
  // Create the Set_option object and execute it.
  std::unique_ptr<cluster::Set_option> op_cluster_set_option;

  // Validation types due to a limitation on the expose() framework.
  // Currently, it's not possible to do overloading of functions that overload
  // an argument of type string/int since the type int is convertible to
  // string, thus overloading becomes ambiguous. As soon as that limitation is
  // gone, this type checking shall go away too.
  if (value.type == shcore::String) {
    std::string value_str = value.as_string();
    op_cluster_set_option =
        std::make_unique<cluster::Set_option>(this, option, value_str);
  } else if (value.type == shcore::Integer || value.type == shcore::UInteger) {
    int64_t value_int = value.as_int();
    op_cluster_set_option =
        std::make_unique<cluster::Set_option>(this, option, value_int);
  } else if (value.type == shcore::Bool) {
    bool value_bool = value.as_bool();
    op_cluster_set_option =
        std::make_unique<cluster::Set_option>(this, option, value_bool);
  } else {
    throw shcore::Exception::type_error(
        "Argument #2 is expected to be a string, an integer or a boolean.");
  }

  // Always execute finish when leaving "try catch".
  shcore::on_leave_scope finally(
      [&op_cluster_set_option]() { op_cluster_set_option->finish(); });

  // Prepare the Set_option command execution (validations).
  op_cluster_set_option->prepare();

  // Execute Set_option operations.
  op_cluster_set_option->execute();
}

mysqlshdk::mysql::Lock_scoped Cluster_impl::get_lock_shared(
    std::chrono::seconds timeout) {
  return get_lock(mysqlshdk::mysql::Lock_mode::SHARED, timeout);
}

mysqlshdk::mysql::Lock_scoped Cluster_impl::get_lock_exclusive(
    std::chrono::seconds timeout) {
  return get_lock(mysqlshdk::mysql::Lock_mode::EXCLUSIVE, timeout);
}

const Cluster_set_member_metadata &Cluster_impl::get_clusterset_metadata()
    const {
  if (m_cs_md.cluster_set_id.empty()) {
    m_metadata_storage->get_cluster_set_member(get_id(), &m_cs_md);
  }
  return m_cs_md;
}

bool Cluster_impl::is_cluster_set_member(const std::string &cs_id) const {
  if (m_cs_md_remove_pending) return false;

  bool is_member = !get_clusterset_metadata().cluster_set_id.empty();

  if (is_member && !cs_id.empty()) {
    is_member = get_clusterset_metadata().cluster_set_id == cs_id;
  }

  return is_member;
}

void Cluster_impl::invalidate_cluster_set_metadata_cache() {
  m_cs_md.cluster_set_id.clear();
}

bool Cluster_impl::is_invalidated() const {
  if (!is_cluster_set_member()) throw std::logic_error("internal error");

  return get_clusterset_metadata().invalidated;
}

bool Cluster_impl::is_primary_cluster() const {
  if (!is_cluster_set_member()) throw std::logic_error("internal error");

  return get_clusterset_metadata().primary_cluster;
}

void Cluster_impl::handle_notification(const std::string &name,
                                       const shcore::Object_bridge_ref &,
                                       shcore::Value::Map_type_ref) {
  if (name == kNotifyClusterSetPrimaryChanged) {
    invalidate_cluster_set_metadata_cache();
  }
}

mysqlshdk::mysql::Lock_scoped Cluster_impl::get_lock(
    mysqlshdk::mysql::Lock_mode mode, std::chrono::seconds timeout) {
  if (!m_cluster_server->is_lock_service_installed()) {
    bool lock_service_installed = false;
    // if SRO is disabled, we have a chance to install the lock service
    if (bool super_read_only =
            m_cluster_server->get_sysvar_bool("super_read_only", false);
        !super_read_only) {
      // we must disable log_bin to prevent the installation from being
      // replicated
      mysqlshdk::mysql::Suppress_binary_log nobinlog(m_cluster_server.get());

      lock_service_installed =
          m_cluster_server->ensure_lock_service_is_installed(false);
    }

    if (!lock_service_installed) {
      log_warning(
          "The required MySQL Locking Service isn't installed on instance "
          "'%s'. The operation will continue without concurrent execution "
          "protection.",
          m_cluster_server->descr().c_str());
      return nullptr;
    }
  }

  DBUG_EXECUTE_IF("dba_locking_timeout_one",
                  { timeout = std::chrono::seconds{1}; });

  // Try to acquire the specified lock.
  //
  // NOTE: Only one lock per namespace is used because lock release is performed
  // by namespace.
  try {
    log_debug("Acquiring %s lock ('%s', '%s') on '%s'.",
              mysqlshdk::mysql::to_string(mode).c_str(), k_lock_ns, k_lock_name,
              m_cluster_server->descr().c_str());
    mysqlshdk::mysql::get_lock(*m_cluster_server, k_lock_ns, k_lock_name, mode,
                               timeout.count());
  } catch (const shcore::Error &err) {
    // Abort the operation in case the required lock cannot be acquired.
    log_info("Failed to get %s lock ('%s', '%s') on '%s': %s",
             mysqlshdk::mysql::to_string(mode).c_str(), k_lock_ns, k_lock_name,
             m_cluster_server->descr().c_str(), err.what());

    if (err.code() == ER_LOCKING_SERVICE_TIMEOUT) {
      current_console()->print_error(shcore::str_format(
          "The operation cannot be executed because it failed to acquire the "
          "Cluster lock through primary member '%s'. Another operation "
          "requiring access to the member is still in progress, please wait "
          "for it to finish and try again.",
          m_cluster_server->descr().c_str()));
      throw shcore::Exception(
          shcore::str_format(
              "Failed to acquire Cluster lock through primary member '%s'",
              m_cluster_server->descr().c_str()),
          SHERR_DBA_LOCK_GET_FAILED);
    } else {
      current_console()->print_error(shcore::str_format(
          "The operation cannot be executed because it failed to acquire the "
          "cluster lock through primary member '%s': %s",
          m_cluster_server->descr().c_str(), err.what()));

      throw;
    }
  }

  auto release_cb = [instance = m_cluster_server]() {
    // Release all instance locks in the k_lock_ns namespace.
    //
    // NOTE: Only perform the operation if the lock service is
    // available, otherwise do nothing (ignore if concurrent execution is not
    // supported, e.g., lock service plugin not available).
    try {
      log_debug("Releasing locks for '%s' on %s.", k_lock_ns,
                instance->descr().c_str());
      mysqlshdk::mysql::release_lock(*instance, k_lock_ns);

    } catch (const shcore::Error &error) {
      // Ignore any error trying to release locks (e.g., might have not
      // been previously acquired due to lack of permissions).
      log_error("Unable to release '%s' locks for '%s': %s", k_lock_ns,
                instance->descr().c_str(), error.what());
    }
  };

  return mysqlshdk::mysql::Lock_scoped{std::move(release_cb)};
}

}  // namespace dba
}  // namespace mysqlsh
