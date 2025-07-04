/*
 * Copyright (c) 2019, 2024, Oracle and/or its affiliates.
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

#include <array>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "modules/adminapi/common/common.h"
#include "modules/adminapi/common/group_replication_options.h"
#include "modules/adminapi/common/server_features.h"
#include "mysqlshdk/include/scripting/type_info/custom.h"
#include "mysqlshdk/include/scripting/type_info/generic.h"
#include "mysqlshdk/libs/mysql/group_replication.h"
#include "mysqlshdk/libs/utils/utils_net.h"

namespace mysqlsh {
namespace dba {

/**
 * Validate the value specified for the localAddress option.
 *
 * @param local_address string containing the value we want to set for
 * gr_local_address
 * @throw ArgumentError if the value is empty or no host and port is specified
 *        (i.e., value is ":").
 */
void validate_local_address_option(std::string local_address,
                                   const std::string &communication_stack,
                                   int canonical_port) {
  // Minimal validation is performed here, the rest is already currently
  // handled at resolve_gr_local_address() (including the logic to automatically
  // set the host and port when not specified).

  local_address = shcore::str_strip(local_address);
  if (local_address.empty()) {
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for %s, string value cannot be empty.", kLocalAddress));
  }

  if (local_address.compare(":") == 0) {
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for %s. If ':' is specified then at least a "
        "non-empty host or port must be specified: '<host>:<port>' or "
        "'<host>:' or ':<port>'.",
        kLocalAddress));
  }

  // If the communicationStack is 'MySQL', the port must be no other than
  // the use in use by MySQL. If 'XCOM', the check doesn't apply
  if (communication_stack != kCommunicationStackMySQL) return;

  // Parse the given address host:port (both parts are optional).
  // Note: Get the last ':' in case a IPv6 address is used, e.g. [::1]:123.
  size_t pos = local_address.find_last_of(":");
  std::string str_local_port;

  if (pos == std::string::npos) {
    // No separator found ':'.
    // If the value only has digits assume it is a port, otherwise, it's the
    // hostname which is not validated here
    if (std::all_of(local_address.cbegin(), local_address.cend(), ::isdigit)) {
      str_local_port = local_address;
    }
  } else {
    // Local address with ':' separating host and port.
    str_local_port = local_address.substr(pos + 1, local_address.length());
  }

  // If it's empty ignore since it'll re resolved later
  if (!str_local_port.empty()) {
    int local_port;
    // Convert port string value to int
    try {
      local_port = std::stoi(str_local_port);
    } catch (const std::exception &) {
      // Error if the port cannot be converted to an integer (not valid).
      throw shcore::Exception::argument_error(
          "Invalid port '" + str_local_port +
          "' for localAddress option. The port must be an integer between "
          "1 and 65535.");
    }

    if (local_port != canonical_port) {
      throw shcore::Exception::argument_error(
          "Invalid port '" + str_local_port +
          "' for localAddress option. When using '" + kCommunicationStackMySQL +
          "' communication stack, the port must be the same in use by "
          "MySQL Server");
    }
  }
}

/**
 * Validate the value specified for the exitStateAction option is supported on
 * the target instance
 *
 * @param session object which represents the session to the instance
 * @throw RuntimeError if the value is not supported on the target instance
 */
void validate_exit_state_action_supported(
    const mysqlshdk::utils::Version &version, std::string exit_state_action) {
  // TODO(rennox): Remove the empty validation
  exit_state_action = shcore::str_strip(exit_state_action);

  if (exit_state_action.empty())
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for %s, string value cannot be empty.",
        kExitStateAction));

  if (!is_option_supported(version, kExitStateAction,
                           k_global_cluster_supported_options)) {
    throw shcore::Exception::runtime_error(shcore::str_format(
        "Option '%s' not supported on target server version: '%s'",
        kExitStateAction, version.get_full().c_str()));
  }
}

/**
 * Validate if the consistency option is supported the target instance
 * version. The actual value is validated by the GR plugin.
 *
 * @param version version of the target instance
 * @throw RuntimeError if the value is not supported on the target instance
 * @throw argument_error if the value provided is empty
 */
void validate_consistency_supported(
    const mysqlshdk::utils::Version &version,
    const std::optional<std::string> &consistency) {
  // TODO(rennox): Remove the empty validation
  if (!consistency.has_value()) return;

  if (shcore::str_strip(*consistency).empty()) {
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for %s, string value cannot be empty.", kConsistency));
  }

  if (!is_option_supported(version, kConsistency,
                           k_global_cluster_supported_options)) {
    throw std::runtime_error(shcore::str_format(
        "Option '%s' not supported on target server version: '%s'",
        kConsistency, version.get_full().c_str()));
  }
}

/**
 * Validate if the expelTimeout option is supported in the target instance
 * version and within the accepted range. The actual value is validated by
 * the GR plugin.
 *
 * @param session object which represents the session to the instance
 * @param expel_timeout nullable object with the value of expelTimeout
 * @throw RuntimeError if the value is not supported on the target instance
 * @throw argument_error if the value provided not within the valid range.
 */
void validate_expel_timeout_supported(
    const mysqlshdk::utils::Version &version,
    std::optional<std::int64_t> expel_timeout) {
  if (!expel_timeout.has_value()) return;

  if ((*expel_timeout) < 0) {
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for '%s': integer value must be >= 0", kExpelTimeout));
  }

  if (!is_option_supported(version, kExpelTimeout,
                           k_global_cluster_supported_options)) {
    throw std::runtime_error(shcore::str_format(
        "Option '%s' not supported on target server version: '%s'",
        kExpelTimeout, version.get_full().c_str()));
  }

  if ((version < mysqlshdk::utils::Version(8, 0, 14)) &&
      (*expel_timeout > 3600)) {
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for '%s': integer value must be in the range [0, 3600]",
        kExpelTimeout));
  }
}

/**
 * Validate the value specified for the autoRejoinTries option is supported on
 * the target instance version. The actual value is validated by the GR plugin.
 *
 * @param version version of the target instance
 * @throw RuntimeError if the value is not supported on the target instance
 */
void validate_auto_rejoin_tries_supported(
    const mysqlshdk::utils::Version &version) {
  // The rejoinRetries option shall only be allowed if the target MySQL
  // server version is >= 8.0.16.
  if (!is_option_supported(version, kAutoRejoinTries,
                           k_global_cluster_supported_options)) {
    throw shcore::Exception::runtime_error(shcore::str_format(
        "Option '%s' not supported on target server version: '%s'",
        kAutoRejoinTries, version.get_full().c_str()));
  }
}

/**
 * Validate the value specified for the memberWeight option is supported on
 * the target instance
 *
 * @param options Map type value with containing the specified options.
 * @param version version of the target instance
 * @throw RuntimeError if the value is not supported on the target instance
 */
void validate_member_weight_supported(
    const mysqlshdk::utils::Version &version) {
  // The memberWeight option shall only be allowed if the target MySQL
  // server version is >= 5.7.20 if 5.0, or >= 8.0.11 if 8.0.

  if (!is_option_supported(version, kMemberWeight,
                           k_global_cluster_supported_options)) {
    throw shcore::Exception::runtime_error(shcore::str_format(
        "Option '%s' not supported on target server version: '%s'",
        kMemberWeight, version.get_full().c_str()));
  }
}

/**
 * Validates the ipWhitelist option
 *
 * Checks if the given ipWhitelist is valid for use in the AdminAPI.
 *
 * @param version version of the target instance
 * @param ip_whitelist The ipWhitelist to validate
 * @param ip_allowlist_option_name The allowList option name used
 *
 * @throws argument_error if the ipWhitelist is empty
 * @throws argument_error if the subnet in CIDR notation is not valid
 * @throws argument_error if the address if the address is a IPv6 and the server
 * version does not support it.
 * @throws argument_error if the address if the address is an hostname and the
 * server version does not support it.
 */
void validate_ip_whitelist_option(const mysqlshdk::utils::Version &version,
                                  const std::string &ip_whitelist,
                                  const std::string &ip_allowlist_option_name) {
  const bool hostnames_supported =
      version >= mysqlshdk::utils::Version(8, 0, 4);
  const bool supports_ipv6 = version >= mysqlshdk::utils::Version(8, 0, 14);

  // Validate if the ipWhiteList value is not empty
  if (shcore::str_strip(ip_whitelist).empty())
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for %s: string value cannot be empty.",
        ip_allowlist_option_name.c_str()));

  // Iterate over the ipWhitelist values
  const std::vector<std::string> ip_whitelist_list =
      shcore::str_split(ip_whitelist, ",", -1);

  for (const std::string &item : ip_whitelist_list) {
    // Strip any blank chars from the ip_whitelist value
    const std::string hostname = shcore::str_strip(item);

    // Check if a subnet using CIDR notation was used and validate its value
    // and separate the address
    //
    // CIDR notation is a compact representation of an IP address and its
    // associated routing prefix. The notation is constructed from an IP
    // address, a slash ('/') character, and a decimal number. The number is the
    // count of leading 1 bits in the routing mask, traditionally called the
    // network mask. The IP address is expressed according to the standards of
    // IPv4 or IPv6.

    const auto ip = mysqlshdk::utils::Net::strip_cidr(hostname);
    const bool is_ipv6 = mysqlshdk::utils::Net::is_ipv6(std::get<0>(ip));
    const bool is_ipv4 = mysqlshdk::utils::Net::is_ipv4(std::get<0>(ip));
    const bool is_hostname = !is_ipv4 && !is_ipv6;

    if (const auto &cidr = std::get<1>(ip); cidr) {
      // Check if value is an hostname: hostname/cidr is not allowed
      if (is_hostname)
        throw shcore::Exception::argument_error(shcore::str_format(
            "Invalid value for %s '%s': CIDR notation can only be used with "
            "IPv4 or IPv6 addresses.",
            ip_allowlist_option_name.c_str(), hostname.c_str()));

      if ((*cidr < 1) || (*cidr > 128))
        throw shcore::Exception::argument_error(shcore::str_format(
            "Invalid value for %s '%s': subnet value in CIDR notation is not "
            "valid.",
            ip_allowlist_option_name.c_str(), hostname.c_str()));

      if (*cidr > 32) {
        if (!supports_ipv6)
          throw shcore::Exception::argument_error(shcore::str_format(
              "Invalid value for %s '%s': subnet value in CIDR notation is not "
              "supported (version >= 8.0.14 required for IPv6 support).",
              ip_allowlist_option_name.c_str(), hostname.c_str()));

        if (!is_ipv6)
          throw shcore::Exception::argument_error(shcore::str_format(
              "Invalid value for %s '%s': subnet value in CIDR notation is not "
              "supported for IPv4 addresses.",
              ip_allowlist_option_name.c_str(), hostname.c_str()));
      }
    }
    // Do not try to resolve the hostname. Even if we can resolve it, there is
    // no guarantee that name resolution is the same across cluster instances
    // and the instance where we are running the ngshell.

    if (is_hostname && !hostnames_supported) {
      throw shcore::Exception::argument_error(shcore::str_format(
          "Invalid value for %s '%s': hostnames are not supported (version >= "
          "8.0.4 required for hostname support).",
          ip_allowlist_option_name.c_str(), hostname.c_str()));
    } else {
      if (!supports_ipv6 && is_ipv6) {
        throw shcore::Exception::argument_error(shcore::str_format(
            "Invalid value for %s '%s': IPv6 not supported (version >= 8.0.14 "
            "required for IPv6 support).",
            ip_allowlist_option_name.c_str(), hostname.c_str()));
      }
    }
    // either an IPv4 address, a supported IPv6 or an hostname
  }
}

/**
 * Validate the value specified for the communicationStack option is supported
 * on the target instance
 *
 * @param version version of the target instance
 * @throw RuntimeError if the value is not supported on the target instance
 */
void validate_communication_stack_supported(
    const mysqlshdk::utils::Version &version) {
  // The communicationStack option must be allowed only if the target
  // server version is >= 8.0.27
  if (!supports_mysql_communication_stack(version)) {
    throw shcore::Exception::runtime_error(shcore::str_format(
        "Option '%s' not supported on target server version: '%s'",
        kCommunicationStack, version.get_full().c_str()));
  }
}

/**
 * Validate if setting paxosSingleLeader is supported on the target instance
 *
 * @param version version of the target instance
 * @throw RuntimeError if the value is not supported on the target instance
 */
void validate_paxos_single_leader_supported(
    const mysqlshdk::utils::Version &version) {
  // The paxosSingleLeader option must be allowed only if the target
  // server version is >= 8.0.31
  // The option handling relies on the column
  // `WRITE_CONSENSUS_SINGLE_LEADER_CAPABLE` of
  // `performance_schema.replication_group_communication_information` that was
  // added in 8.0.31
  if (!supports_paxos_single_leader(version)) {
    throw shcore::Exception::runtime_error(shcore::str_format(
        "Option '%s' not supported on target server version: '%s'",
        kPaxosSingleLeader, version.get_full().c_str()));
  }
}

// ----

void Group_replication_options::check_option_values(
    const mysqlshdk::utils::Version &version, int canonical_port) {
  // Validate communicationStack
  if (communication_stack.has_value()) {
    validate_communication_stack_supported(version);
  }

  // Validate paxosSingleLeader
  if (paxos_single_leader.has_value()) {
    validate_paxos_single_leader_supported(version);
  }

  // Validate ipWhitelist and ipAllowlist
  {
    if (ip_allowlist.has_value()) {
      if (!shcore::str_caseeq(*ip_allowlist, "AUTOMATIC")) {
        validate_ip_whitelist_option(version, *ip_allowlist,
                                     ip_allowlist_option_name);
      }
    }
  }

  if (local_address.has_value()) {
    validate_local_address_option(
        *local_address, communication_stack.value_or(""), canonical_port);
  }

  // Validate if the exitStateAction option is supported on the target
  // instance and if is not empty.
  // The validation for the value set is handled at the group-replication
  // level
  if (exit_state_action.has_value()) {
    validate_exit_state_action_supported(version, *exit_state_action);
  } else {
    // exitStateAction default value should only be set if supported in
    // the target instance and must be READ_ONLY for versions < 8.0.16 since
    // the default was ABORT_SERVER which was considered to be too drastic
    // NOTE: In 8.0.16 the default value became READ_ONLY so we must not change
    // it
    if (target == JOIN &&
        is_option_supported(version, kExitStateAction,
                            k_global_cluster_supported_options) &&
        version < mysqlshdk::utils::Version("8.0.16")) {
      exit_state_action = "READ_ONLY";
    }
  }

  // Validate if the memberWeight option is supported on the target
  // instance and if it used in the optional parameters.
  // The validation for the value set is handled at the group-replication
  // level
  if (member_weight.has_value()) {
    validate_member_weight_supported(version);
  }

  if (consistency.has_value()) {
    // Validate if the consistency option is supported on the target
    // instance and if is not empty.
    // The validation for the value set is handled at the group-replication
    // level
    validate_consistency_supported(version, consistency);
  }

  if (expel_timeout.has_value()) {
    // Validate if the expelTimeout option is supported on the target
    // instance and if it is within the valid range [0, 3600].
    validate_expel_timeout_supported(version, expel_timeout);
  }

  if (auto_rejoin_tries.has_value()) {
    // Validate if the auto_rejoin_tries option is supported on the target
    validate_auto_rejoin_tries_supported(version);

    // Print warning if auto-rejoin is set (not 0).
    if (*auto_rejoin_tries != 0) {
      if (auto console = mysqlsh::current_console(true); console) {
        console->print_warning(
            "The member will only proceed according to its exitStateAction if "
            "auto-rejoin fails (i.e. all retry attempts are exhausted).");
        console->print_info();
      }
    }
  }
}

void Group_replication_options::read_option_values(
    const mysqlshdk::mysql::IInstance &instance, bool switching_comm_stack) {
  mysqlshdk::utils::Version version = instance.get_version();

  if (!group_name.has_value()) {
    group_name = instance.get_sysvar_string("group_replication_group_name");
  }

  if (ssl_mode == Cluster_ssl_mode::NONE) {
    ssl_mode = to_cluster_ssl_mode(
        instance.get_sysvar_string("group_replication_ssl_mode").value_or(""));
  }

  if (!group_seeds.has_value() && !switching_comm_stack) {
    group_seeds = instance.get_sysvar_string("group_replication_group_seeds");

    // Set group_seeds to NULL if value read is empty (to be overridden).
    if (group_seeds.has_value() && group_seeds->empty()) group_seeds.reset();
  }

  if (!ip_allowlist.has_value() && !switching_comm_stack) {
    if (version < mysqlshdk::utils::Version(8, 0, 23)) {
      ip_allowlist =
          instance.get_sysvar_string("group_replication_ip_whitelist");
    } else {
      ip_allowlist =
          instance.get_sysvar_string("group_replication_ip_allowlist");
    }
  }

  if (!local_address.has_value() && !switching_comm_stack) {
    local_address =
        instance.get_sysvar_string("group_replication_local_address");
    // group_replication_local_address will be "" when it's not set, but we
    // don't allow setting it to ""
    if (local_address.has_value() && *local_address == "")
      local_address = std::nullopt;
  }

  if (!member_weight.has_value() &&
      version >= mysqlshdk::utils::Version(8, 0, 2)) {
    member_weight = instance.get_sysvar_int("group_replication_member_weight");
  }

  if (!exit_state_action.has_value() &&
      version >= mysqlshdk::utils::Version(8, 0, 12)) {
    exit_state_action =
        instance.get_sysvar_string("group_replication_exit_state_action");
  }

  if (!expel_timeout.has_value() &&
      version >= mysqlshdk::utils::Version(8, 0, 13)) {
    expel_timeout =
        instance.get_sysvar_int("group_replication_member_expel_timeout");
  }

  if (!consistency.has_value() &&
      version >= mysqlshdk::utils::Version(8, 0, 14)) {
    consistency = instance.get_sysvar_string("group_replication_consistency");
  }

  if (!auto_rejoin_tries.has_value() &&
      version >= mysqlshdk::utils::Version(8, 0, 16)) {
    auto_rejoin_tries =
        instance.get_sysvar_int("group_replication_autorejoin_tries");
  }
}

void Group_replication_options::set_local_address(const std::string &value) {
  local_address = shcore::str_strip(value);

  if (local_address->empty()) {
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for %s, string value cannot be empty.", kLocalAddress));
  }

  if ((*local_address).compare(":") == 0) {
    throw shcore::Exception::argument_error(
        shcore::str_format("Invalid value for %s. If ':' is specified then at "
                           "least a non-empty host or port must be specified: "
                           "'<host>:<port>' or '<host>:' or ':<port>'.",
                           kLocalAddress));
  }
}

void Group_replication_options::set_exit_state_action(
    const std::string &value) {
  exit_state_action = shcore::str_strip(value);

  if (exit_state_action->empty()) {
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for %s, string value cannot be empty.",
        kExitStateAction));
  }
}

void Group_replication_options::set_member_weight(int64_t value) {
  member_weight = value;
}

void Group_replication_options::set_auto_rejoin_tries(int64_t value) {
  auto_rejoin_tries = value;
}

void Group_replication_options::set_expel_timeout(int64_t value) {
  if (value < 0) {
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for '%s': integer value must be >= 0", kExpelTimeout));
  }
  expel_timeout = value;
}

void Group_replication_options::set_consistency(const std::string &option,
                                                const std::string &value) {
  auto stripped_value = shcore::str_strip(value);

  if (stripped_value.empty()) {
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for '%s', string value cannot be empty.",
        option.c_str()));
  }

  if (option == kFailoverConsistency) {
    handle_deprecated_option(kFailoverConsistency, kConsistency,
                             consistency.has_value(), false);
  }

  consistency = stripped_value;
}

void Group_replication_options::set_communication_stack(
    const std::string &value) {
  communication_stack = shcore::str_upper(shcore::str_strip(value));

  if (communication_stack->empty()) {
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for '%s' option. String value cannot be empty.",
        kCommunicationStack));
  }

  if (kCommunicationStackValidValues.count(shcore::str_upper(value)) == 0) {
    std::string valid_values =
        shcore::str_join(kCommunicationStackValidValues, ", ");
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for '%s' option. Supported values: %s.",
        kCommunicationStack, valid_values.c_str()));
  }
}

void Group_replication_options::set_transaction_size_limit(int64_t value) {
  transaction_size_limit = value;
}

void Group_replication_options::set_paxos_single_leader(bool value) {
  paxos_single_leader = value;
}

const shcore::Option_pack_def<Rejoin_group_replication_options>
    &Rejoin_group_replication_options::options() {
  static const auto opts =
      shcore::Option_pack_def<Rejoin_group_replication_options>()
          .optional(kMemberSslMode,
                    &Rejoin_group_replication_options::set_ssl_mode)
          .optional(kIpAllowlist,
                    &Rejoin_group_replication_options::set_ip_allowlist)
          .optional(kIpWhitelist,
                    &Rejoin_group_replication_options::set_ip_allowlist, "",
                    shcore::Option_extract_mode::CASE_INSENSITIVE,
                    shcore::Option_scope::DEPRECATED)
          .optional(kLocalAddress,
                    &Rejoin_group_replication_options::set_local_address);

  return opts;
}

const shcore::Option_pack_def<Reboot_group_replication_options>
    &Reboot_group_replication_options::options() {
  static const auto opts =
      shcore::Option_pack_def<Reboot_group_replication_options>()
          .optional(kIpAllowlist,
                    &Reboot_group_replication_options::set_ip_allowlist)
          .optional(kLocalAddress,
                    &Reboot_group_replication_options::set_local_address)
          .optional(kPaxosSingleLeader,
                    &Reboot_group_replication_options::set_paxos_single_leader);
  // TODO(miguel): add support for changing the sslMode

  return opts;
}

void Rejoin_group_replication_options::set_ssl_mode(const std::string &value) {
  if (std::find(kClusterSSLModeValues.begin(), kClusterSSLModeValues.end(),
                shcore::str_upper(value)) == kClusterSSLModeValues.end()) {
    std::string valid_values = shcore::str_join(kClusterSSLModeValues, ",");
    throw shcore::Exception::argument_error(
        shcore::str_format("Invalid value for %s option. Supported values: %s.",
                           kMemberSslMode, valid_values.c_str()));
  }

  if (target == Unpack_target::JOIN || target == Unpack_target::REJOIN) {
    auto console = current_console();
    console->print_warning(shcore::str_format(
        "Option '%s' is deprecated for this operation and it will "
        "be removed in a future release. This option is not needed because the "
        "SSL mode is automatically obtained from the cluster. Please do not "
        "use it here.",
        kMemberSslMode));
    console->print_info();
  }

  // Set the ssl-mode
  ssl_mode = to_cluster_ssl_mode(value);
}

void Rejoin_group_replication_options::set_ip_allowlist(
    const std::string &option, const std::string &value) {
  // Validate if the value is not empty
  if (shcore::str_strip(value).empty())
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for %s: string value cannot be empty.", option.c_str()));

  if (option == kIpWhitelist) {
    handle_deprecated_option(kIpWhitelist, kIpAllowlist,
                             ip_allowlist.has_value(), true);
  }

  ip_allowlist = value;
  ip_allowlist_option_name = option;
}

const shcore::Option_pack_def<Join_group_replication_options>
    &Join_group_replication_options::options() {
  static const auto opts =
      shcore::Option_pack_def<Join_group_replication_options>()
          .include<Rejoin_group_replication_options>()
          .optional(kGroupSeeds,
                    &Join_group_replication_options::set_group_seeds)
          .optional(kExitStateAction,
                    &Group_replication_options::set_exit_state_action)
          .optional(kMemberWeight,
                    &Group_replication_options::set_member_weight, "",
                    shcore::Option_extract_mode::EXACT)
          .optional(kAutoRejoinTries,
                    &Group_replication_options::set_auto_rejoin_tries);

  return opts;
}

void Join_group_replication_options::set_group_seeds(const std::string &) {
  handle_deprecated_option(kGroupSeeds, "");
}

const shcore::Option_pack_def<Cluster_set_group_replication_options>
    &Cluster_set_group_replication_options::options() {
  static const auto opts =
      shcore::Option_pack_def<Cluster_set_group_replication_options>()
          .include<Rejoin_group_replication_options>()
          .optional(kExitStateAction,
                    &Group_replication_options::set_exit_state_action)
          .optional(kMemberWeight,
                    &Group_replication_options::set_member_weight, "",
                    shcore::Option_extract_mode::EXACT)
          .optional(
              kManualStartOnBoot,
              &Cluster_set_group_replication_options::manual_start_on_boot)
          .optional(kConsistency, &Group_replication_options::set_consistency)
          .optional(kExpelTimeout,
                    &Group_replication_options::set_expel_timeout, "",
                    shcore::Option_extract_mode::EXACT)
          .optional(kAutoRejoinTries,
                    &Group_replication_options::set_auto_rejoin_tries)
          .optional(kCommunicationStack,
                    &Group_replication_options::set_communication_stack, "",
                    shcore::Option_extract_mode::CASE_INSENSITIVE)
          .optional(kPaxosSingleLeader,
                    &Group_replication_options::set_paxos_single_leader);

  return opts;
}

const shcore::Option_pack_def<Create_group_replication_options>
    &Create_group_replication_options::options() {
  static const auto opts =
      shcore::Option_pack_def<Create_group_replication_options>()
          .include<Join_group_replication_options>()
          .optional(kGroupName,
                    &Create_group_replication_options::set_group_name)
          .optional(kManualStartOnBoot,
                    &Create_group_replication_options::manual_start_on_boot)
          .optional(kConsistency,
                    &Create_group_replication_options::set_consistency)
          .optional(kFailoverConsistency,
                    &Create_group_replication_options::set_consistency, "",
                    shcore::Option_extract_mode::CASE_INSENSITIVE,
                    shcore::Option_scope::DEPRECATED)
          .optional(kExpelTimeout,
                    &Create_group_replication_options::set_expel_timeout, "",
                    shcore::Option_extract_mode::EXACT)
          .optional(kCommunicationStack,
                    &Create_group_replication_options::set_communication_stack,
                    "", shcore::Option_extract_mode::CASE_INSENSITIVE)
          .optional(kTransactionSizeLimit,
                    &Group_replication_options::set_transaction_size_limit, "",
                    shcore::Option_extract_mode::EXACT)
          .optional(kPaxosSingleLeader,
                    &Create_group_replication_options::set_paxos_single_leader);

  return opts;
}

void Create_group_replication_options::set_group_name(
    const std::string &value) {
  group_name = shcore::str_strip(value);

  if (group_name->empty())
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for %s, string value cannot be empty.", kGroupName));
}

}  // namespace dba
}  // namespace mysqlsh
