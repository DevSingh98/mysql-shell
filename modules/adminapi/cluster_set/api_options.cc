/*
 * Copyright (c) 2021, 2024, Oracle and/or its affiliates.
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
#include <string_view>

#include "adminapi/common/api_options.h"
#include "modules/adminapi/cluster_set/api_options.h"
#include "modules/adminapi/common/common.h"
#include "mysqlshdk/include/scripting/type_info/custom.h"
#include "mysqlshdk/include/scripting/type_info/generic.h"

namespace mysqlsh {
namespace dba {
namespace clusterset {

namespace {
constexpr const char *kInvalidateReplicaClusters = "invalidateReplicaClusters";
}  // namespace

const shcore::Option_pack_def<Create_cluster_set_options>
    &Create_cluster_set_options::options() {
  static const auto opts =
      shcore::Option_pack_def<Create_cluster_set_options>()
          .optional(kDryRun, &Create_cluster_set_options::dry_run)
          .optional(kClusterSetReplicationSslMode,
                    &Create_cluster_set_options::set_ssl_mode)
          .optional(kReplicationAllowedHost,
                    &Create_cluster_set_options::replication_allowed_host);

  return opts;
}

void Create_cluster_set_options::set_ssl_mode(const std::string &value) {
  if (std::find(kClusterSSLModeValues.begin(), kClusterSSLModeValues.end(),
                shcore::str_upper(value)) == kClusterSSLModeValues.end()) {
    std::string valid_values = shcore::str_join(kClusterSSLModeValues, ",");
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for %s option. Supported values: %s.",
        kClusterSetReplicationSslMode, valid_values.c_str()));
  }

  // Set the ssl-mode
  ssl_mode = to_cluster_ssl_mode(value);
}

const shcore::Option_pack_def<Create_replica_cluster_options>
    &Create_replica_cluster_options::options() {
  static const auto opts =
      shcore::Option_pack_def<Create_replica_cluster_options>()
          .include<Interactive_option>()
          .include<Timeout_option>()
          .optional(kDryRun, &Create_replica_cluster_options::dry_run)
          .include<Recovery_progress_option>()
          .optional(kReplicationAllowedHost,
                    &Create_replica_cluster_options::replication_allowed_host)
          .include(&Create_replica_cluster_options::gr_options)
          .include(&Create_replica_cluster_options::clone_options)
          .optional(kCertSubject,
                    &Create_replica_cluster_options::set_cert_subject);

  return opts;
}

void Create_replica_cluster_options::set_cert_subject(
    const std::string &value) {
  if (value.empty())
    throw shcore::Exception::argument_error(shcore::str_format(
        "Invalid value for '%s' option. Value cannot be an empty string.",
        kCertSubject));

  cert_subject = value;
}

const shcore::Option_pack_def<Remove_cluster_options>
    &Remove_cluster_options::options() {
  static const auto opts =
      shcore::Option_pack_def<Remove_cluster_options>()
          .include<Timeout_option>()
          .optional(kDryRun, &Remove_cluster_options::dry_run)
          .optional(kForce, &Remove_cluster_options::force);

  return opts;
}

const shcore::Option_pack_def<Status_options> &Status_options::options() {
  static const auto opts = shcore::Option_pack_def<Status_options>().optional(
      kExtended, &Status_options::set_extended);

  return opts;
}

void Status_options::set_extended(uint64_t value) {
  // Validate extended option UInteger [0, 3] or Boolean.
  if (value > 3) {
    throw shcore::Exception::argument_error(
        shcore::str_format("Invalid value '%" PRIu64
                           "' for option '%s'. It must be an integer in the "
                           "range [0, 3].",
                           value, kExtended));
  }

  extended = value;
}

const shcore::Option_pack_def<Invalidate_replica_clusters_option>
    &Invalidate_replica_clusters_option::options() {
  static const auto opts =
      shcore::Option_pack_def<Invalidate_replica_clusters_option>().optional(
          kInvalidateReplicaClusters,
          &Invalidate_replica_clusters_option::set_list_option);

  return opts;
}

void Invalidate_replica_clusters_option::set_list_option(
    const std::string &option, const shcore::Value &value) {
  assert(option == kInvalidateReplicaClusters);

  if (value.type == shcore::Value_type::Array) {
    auto array = value.as_array();
    if (array->empty()) {
      throw shcore::Exception::argument_error(shcore::str_format(
          "The list for '%s' option cannot be empty.", option.c_str()));
    }

    invalidate_replica_clusters =
        value.to_string_container<std::list<std::string>>();

  } else {
    throw shcore::Exception::argument_error(shcore::str_format(
        "The '%s' option must be a list of strings.", option.c_str()));
  }
}

const shcore::Option_pack_def<Set_primary_cluster_options>
    &Set_primary_cluster_options::options() {
  static const auto opts =
      shcore::Option_pack_def<Set_primary_cluster_options>()
          .include<Invalidate_replica_clusters_option>()
          .include<Timeout_option>()
          .optional(kDryRun, &Set_primary_cluster_options::dry_run);

  return opts;
}

const shcore::Option_pack_def<Force_primary_cluster_options>
    &Force_primary_cluster_options::options() {
  static const auto opts =
      shcore::Option_pack_def<Force_primary_cluster_options>()
          .include<Invalidate_replica_clusters_option>()
          .optional(kDryRun, &Force_primary_cluster_options::dry_run)
          .optional(kTimeout, &Force_primary_cluster_options::set_timeout, "",
                    shcore::Option_extract_mode::CASE_INSENSITIVE);

  return opts;
}

void Force_primary_cluster_options::set_timeout(uint32_t timeout_seconds) {
  timeout = std::chrono::seconds{timeout_seconds};
}

std::chrono::seconds Force_primary_cluster_options::get_timeout() const {
  return timeout.value_or(std::chrono::seconds{
      current_shell_options()->get().dba_gtid_wait_timeout});
}

const shcore::Option_pack_def<Rejoin_cluster_options>
    &Rejoin_cluster_options::options() {
  static const auto opts =
      shcore::Option_pack_def<Rejoin_cluster_options>().optional(
          kDryRun, &Rejoin_cluster_options::dry_run);

  return opts;
}

}  // namespace clusterset
}  // namespace dba

}  // namespace mysqlsh
