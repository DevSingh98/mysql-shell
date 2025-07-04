/*
 * Copyright (c) 2020, 2024, Oracle and/or its affiliates.
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

#ifndef MODULES_UTIL_DUMP_DUMP_OPTIONS_H_
#define MODULES_UTIL_DUMP_DUMP_OPTIONS_H_

#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "mysqlshdk/include/scripting/types.h"
#include "mysqlshdk/include/scripting/types_cpp.h"
#include "mysqlshdk/libs/db/session.h"
#include "mysqlshdk/libs/storage/compressed_file.h"
#include "mysqlshdk/libs/storage/config.h"
#include "mysqlshdk/libs/utils/utils_general.h"
#include "mysqlshdk/libs/utils/version.h"

#include "modules/util/common/dump/filtering_options.h"
#include "modules/util/dump/compatibility_option.h"
#include "modules/util/dump/instance_cache.h"
#include "modules/util/import_table/dialect.h"

namespace mysqlsh {
namespace dump {

class Dump_options {
 public:
  Dump_options();

  Dump_options(const Dump_options &) = default;
  Dump_options(Dump_options &&) = default;

  Dump_options &operator=(const Dump_options &) = default;
  Dump_options &operator=(Dump_options &&) = default;

  virtual ~Dump_options() = default;

  static const shcore::Option_pack_def<Dump_options> &options();

  void validate() const;

  // setters
  void set_session(const std::shared_ptr<mysqlshdk::db::ISession> &session);

  // getters
  const std::string &output_url() const { return m_output_url; }

  void set_output_url(const std::string &url) { m_output_url = url; }

  const shcore::Dictionary_t &original_options() const { return m_options; }

  bool use_base64() const { return m_use_base64; }

  int64_t max_rate() const { return m_max_rate; }

  bool show_progress() const { return m_show_progress; }

  mysqlshdk::storage::Compression compression() const { return m_compression; }

  const std::shared_ptr<mysqlshdk::db::ISession> &session() const {
    return m_session;
  }

  const import_table::Dialect &dialect() const { return m_dialect; }

  const mysqlshdk::storage::Config_ptr &storage_config() const {
    return m_storage_config;
  }

  const std::string &character_set() const { return m_character_set; }

  const std::optional<mysqlshdk::utils::Version> &mds_compatibility() const {
    return m_mds;
  }

  const Compatibility_options &compatibility_options() const {
    return m_compatibility_options;
  }

  common::Filtering_options &filters() { return m_filtering_options; }
  const common::Filtering_options &filters() const {
    return m_filtering_options;
  }

  const Instance_cache_builder::Partition_filters &included_partitions() const {
    return m_partitions;
  }

  const std::string &where(const std::string &schema,
                           const std::string &table) const;

  virtual bool split() const = 0;

  virtual uint64_t bytes_per_chunk() const = 0;

  virtual std::size_t threads() const = 0;

  virtual bool is_export_only() const = 0;

  virtual bool use_single_file() const = 0;

  virtual bool dump_ddl() const = 0;

  virtual bool dump_data() const = 0;

  virtual bool is_dry_run() const = 0;

  virtual bool consistent_dump() const = 0;

  virtual bool skip_consistency_checks() const = 0;

  virtual bool dump_events() const = 0;

  virtual bool dump_routines() const = 0;

  virtual bool dump_triggers() const = 0;

  virtual bool dump_users() const = 0;

  virtual bool use_timezone_utc() const = 0;

  virtual bool dump_binlog_info() const = 0;

  virtual bool par_manifest() const = 0;

 protected:
  void set_compression(mysqlshdk::storage::Compression compression) {
    m_compression = compression;
  }

  void set_mds_compatibility(
      const std::optional<mysqlshdk::utils::Version> &mds) {
    m_mds = mds;
  }

  void set_compatibility_option(Compatibility_option c) {
    m_compatibility_options |= c;
  }

  void set_where_clause(const std::map<std::string, std::string> &where);

  void set_where_clause(const std::string &schema, const std::string &table,
                        const std::string &where);

  void set_partitions(
      const std::map<std::string, std::unordered_set<std::string>> &partitions);

  void set_partitions(const std::string &schema, const std::string &table,
                      const std::unordered_set<std::string> &partitions);

  bool exists(const std::string &schema) const;

  bool exists(const std::string &schema, const std::string &table) const;

  std::set<std::string> find_missing(
      const std::unordered_set<std::string> &schemas) const;

  std::set<std::string> find_missing(
      const std::string &schema,
      const std::unordered_set<std::string> &tables) const;

  // currently used by dumpTables(), dumpSchemas() and dumpInstance()
  common::Filtering_options m_filtering_options;

  mutable bool m_filter_conflicts = false;

 protected:
  void on_start_unpack(const shcore::Dictionary_t &options);
  void set_storage_config(
      const std::shared_ptr<mysqlshdk::storage::Config> &storage_config);

  // This function should be implemented when the validation process requires
  // data NOT coming on the user options, i.e. a session
  virtual void validate_options() const {}

  void on_log_options(const char *msg) const;

 private:
  void on_unpacked_options();

  virtual void on_set_session(
      const std::shared_ptr<mysqlshdk::db::ISession> &session) = 0;

  void set_string_option(const std::string &option, const std::string &value);

  std::set<std::string> find_missing_impl(
      const std::string &subquery,
      const std::unordered_set<std::string> &objects) const;

  void validate_partitions() const;

  // global session
  std::shared_ptr<mysqlshdk::db::ISession> m_session;

  // input arguments
  std::string m_output_url;
  shcore::Dictionary_t m_options;

  // not configurable
  bool m_use_base64 = true;

  // common options
  int64_t m_max_rate = 0;
  bool m_show_progress;
  mysqlshdk::storage::Compression m_compression =
      mysqlshdk::storage::Compression::ZSTD;
  mysqlshdk::storage::Config_ptr m_storage_config;

  std::string m_character_set = "utf8mb4";

  import_table::Dialect m_dialect;
  import_table::Dialect m_dialect_unpacker;

  // schema -> table -> condition
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>>
      m_where;

  // schema -> table -> partitions
  Instance_cache_builder::Partition_filters m_partitions;

  // these options are unpacked elsewhere, but are here 'cause we're returning
  // a reference

  // currently used by dumpTables(), dumpSchemas() and dumpInstance()
  std::optional<mysqlshdk::utils::Version> m_mds;
  Compatibility_options m_compatibility_options;
};

}  // namespace dump
}  // namespace mysqlsh

#endif  // MODULES_UTIL_DUMP_DUMP_OPTIONS_H_
