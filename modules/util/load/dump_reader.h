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

#ifndef MODULES_UTIL_LOAD_DUMP_READER_H_
#define MODULES_UTIL_LOAD_DUMP_READER_H_

#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "modules/util/dump/compatibility.h"
#include "modules/util/dump/progress_thread.h"

#include "modules/util/import_table/dialect.h"

#include "modules/util/load/load_dump_options.h"

#include "mysqlshdk/libs/storage/idirectory.h"
#include "mysqlshdk/libs/storage/ifile.h"
#include "mysqlshdk/libs/utils/thread_pool.h"
#include "mysqlshdk/libs/utils/version.h"

namespace mysqlsh {

class Dump_reader {
 public:
  using Name_and_file =
      std::pair<std::string, std::shared_ptr<mysqlshdk::storage::IFile>>;
  using Files = std::unordered_set<mysqlshdk::storage::IDirectory::File_info>;

  Dump_reader(std::unique_ptr<mysqlshdk::storage::IDirectory> dump_dir,
              const Load_dump_options &options);

  const std::string &default_character_set() const {
    return m_contents.default_charset;
  }

  const mysqlshdk::utils::Version &dump_version() const {
    return m_contents.dump_version;
  }

  bool mds_compatibility() const { return m_contents.mds_compatibility; }

  bool partial_revokes() const { return m_contents.partial_revokes; }

  bool should_create_pks() const;

  const mysqlshdk::utils::Version &server_version() const {
    return m_contents.server_version;
  }

  const std::string &binlog_file() const { return m_contents.binlog_file; }

  uint64_t binlog_position() const { return m_contents.binlog_position; }

  const std::string &gtid_executed() const { return m_contents.gtid_executed; }

  bool gtid_executed_inconsistent() const {
    return m_contents.gtid_executed_inconsistent;
  }

  bool tz_utc() const { return m_contents.tz_utc; }

  /**
   * Checks whether this is a dump created by an old version of dumpTables(),
   * which has no schema SQL.
   */
  bool table_only() const { return m_contents.table_only; }

  /**
   * Checks whether this is a dump created by any version of dumpTables().
   */
  bool is_dump_tables() const {
    return table_only() || "dumpTables" == m_contents.origin;
  }

  void replace_target_schema(const std::string &schema);

  std::string users_script() const;
  std::string begin_script() const;
  std::string end_script() const;

  bool ready() const { return m_contents.ready(); }

  bool next_schema_and_tables(std::string *out_schema,
                              std::list<Name_and_file> *out_tables,
                              std::list<Name_and_file> *out_view_placeholders);
  bool next_schema_and_views(std::string *out_schema,
                             std::list<Name_and_file> *out_views);

  std::vector<shcore::Account> accounts() const;

  std::vector<std::string> schemas() const;

  bool schema_objects(const std::string &schema,
                      std::vector<std::string> *out_tables,
                      std::vector<std::string> *out_views,
                      std::vector<std::string> *out_triggers,
                      std::vector<std::string> *out_functions,
                      std::vector<std::string> *out_procedures,
                      std::vector<std::string> *out_events);

  std::string fetch_schema_script(const std::string &schema) const;

  void schema_table_triggers(const std::string &schema,
                             std::list<Name_and_file> *out_table_triggers);

  const std::vector<std::string> &deferred_schema_fks(
      const std::string &schema) const;

  const std::vector<std::string> &queries_on_schema_end(
      const std::string &schema) const;

  const std::map<std::string, std::vector<std::string>> tables_without_pk()
      const;

  bool has_tables_without_pk() const;

  bool has_primary_key(const std::string &schema,
                       const std::string &table) const;

  bool next_table_chunk(
      const std::unordered_multimap<std::string, size_t> &tables_being_loaded,
      std::string *out_schema, std::string *out_table,
      std::string *out_partition, bool *out_chunked, size_t *out_chunk_index,
      size_t *out_chunks_total,
      std::unique_ptr<mysqlshdk::storage::IFile> *out_file,
      size_t *out_chunk_size, shcore::Dictionary_t *out_options);

  struct Histogram {
    std::string column;
    size_t buckets;
  };

  bool next_deferred_index(
      std::string *out_schema, std::string *out_table,
      compatibility::Deferred_statements::Index_info **out_indexes);

  bool next_table_analyze(std::string *out_schema, std::string *out_table,
                          std::vector<Histogram> *out_histograms);

  bool data_available() const;

  bool work_available() const;

  size_t dump_size() const { return m_contents.dump_size; }
  size_t total_data_size() const { return m_contents.data_size; }
  size_t filtered_data_size() const;
  size_t table_data_size(const std::string &schema,
                         const std::string &table) const;
  void compute_filtered_data_size();

  uint64_t bytes_per_chunk() const { return m_contents.bytes_per_chunk; }

  uint64_t chunk_size(const std::string &name, bool *valid) const {
    assert(valid);

    const auto it = m_contents.chunk_sizes.find(name);

    if (it == m_contents.chunk_sizes.end()) {
      *valid = false;
      return 0;
    } else {
      *valid = true;
      return it->second;
    }
  }

  void rescan(dump::Progress_thread *progress_thread = nullptr);

  uint64_t add_deferred_statements(const std::string &schema,
                                   const std::string &table,
                                   compatibility::Deferred_statements &&stmts);

  void validate_options();

  size_t tables_with_data() const { return m_tables_and_partitions_to_load; }

  enum class Status {
    INVALID,  // No dump or not enough data to start loading yet
    DUMPING,  // Dump is not done yet
    COMPLETE  // Dump is complete
  };

  Status status() const { return m_dump_status; }

  Status open();

  std::unique_ptr<mysqlshdk::storage::IFile> create_progress_file_handle()
      const;

  void show_metadata() const;

  std::unique_ptr<shcore::Thread_pool> create_thread_pool() const;

  void on_metadata_available() { ++m_metadata_available; }

  uint64_t metadata_available() { return m_metadata_available; }

  void on_metadata_parsed() { ++m_metadata_parsed; }

  uint64_t metadata_parsed() { return m_metadata_parsed; }

  struct Table_info;

  void on_table_metadata_parsed(const Table_info &info);

  uint64_t tables_to_load() { return m_tables_to_load; }

  bool has_partitions() const { return m_dump_has_partitions; }

  bool include_schema(const std::string &schema) const;
  bool include_table(const std::string &schema, const std::string &table) const;
  bool include_event(const std::string &schema, const std::string &event) const;
  bool include_routine(const std::string &schema,
                       const std::string &routine) const;
  bool include_trigger(const std::string &schema, const std::string &table,
                       const std::string &trigger) const;

  void on_chunk_loaded(std::string_view schema, std::string_view table,
                       std::string_view partition);

  void on_index_end(std::string_view schema, std::string_view table);

  void on_analyze_end(std::string_view schema, std::string_view table);

  struct Capability_info {
    std::string id;
    std::string description;
    mysqlshdk::utils::Version version_required;
  };

  const std::vector<Capability_info> &capabilities() const {
    return m_contents.capabilities;
  }

  struct View_info {
    std::string schema;
    std::string table;

    std::string basename;

    bool ready() const { return sql_seen && sql_pre_seen; }

    std::string pre_script_name() const;
    std::string script_name() const;

    void rescan(const Files &files);

    bool sql_seen = false;
    bool sql_pre_seen = false;
  };

  struct Table_data_info {
    Table_info *owner = nullptr;

    std::string partition;
    std::string basename;
    std::string extension;

    bool has_data = true;
    bool chunked = false;
    bool last_chunk_seen = false;

    size_t chunks_seen = 0;
    std::vector<std::optional<mysqlshdk::storage::IDirectory::File_info>>
        available_chunks;
    // number of chunks scheduled to be loaded
    size_t chunks_consumed = 0;
    // number of chunks which were loaded
    size_t chunks_loaded = 0;

    void consume_chunk() {
      ++chunks_consumed;

      // skip zero-sized chunks
      while (has_data_available() &&
             0 == available_chunks[chunks_consumed]->size()) {
        ++chunks_consumed;
        ++chunks_loaded;
      }
    }

    bool has_data_available() const {
      return chunks_consumed < available_chunks.size() &&
             available_chunks[chunks_consumed].has_value();
    }

    size_t bytes_available() const {
      size_t total = 0;

      for (size_t i = chunks_consumed, s = available_chunks.size();
           i < s && available_chunks[i].has_value(); ++i) {
        total += available_chunks[i]->size();
      }
      return total;
    }

    bool data_dumped() const { return all_chunks_are(chunks_seen); }

    bool data_scheduled() const { return all_chunks_are(chunks_consumed); }

    bool data_loaded() const { return all_chunks_are(chunks_loaded); }

    void rescan_data(const Files &files, Dump_reader *reader);

    const std::string &key() const {
      if (m_key.empty()) {
        m_key = schema_table_object_key(owner->schema, owner->table, partition);
      }

      return m_key;
    }

   private:
    bool all_chunks_are(std::size_t what) const {
      return !has_data || (last_chunk_seen && what == available_chunks.size());
    }

    mutable std::string m_key;
  };

  struct Table_info {
    std::string schema;
    std::string table;

    std::string basename;

    std::vector<std::string> primary_index;

    bool has_sql = true;
    volatile bool md_done = false;
    bool sql_seen = false;

    shcore::Dictionary_t options = nullptr;
    compatibility::Deferred_statements::Index_info indexes;
    bool indexes_scheduled = true;
    bool indexes_created = true;
    std::vector<Histogram> histograms;
    bool analyze_scheduled = true;
    bool analyze_finished = true;
    bool has_triggers = false;

    std::vector<Table_data_info> data_info;

    std::string script_name() const;
    std::string triggers_script_name() const;

    bool ready() const;

    std::string metadata_name() const;

    bool should_fetch_metadata_file(const Files &files) const;

    void update_metadata(const std::string &data, Dump_reader *reader);

    void rescan(const Files &files);

    void rescan_data(const Files &files, Dump_reader *reader);

    bool all_data_scheduled() const;

    bool all_data_loaded() const;
  };

  struct Schema_info {
    std::string schema;

    std::string basename;

    std::unordered_map<std::string, std::shared_ptr<Table_info>> tables;
    std::list<View_info> views;
    std::vector<std::string> trigger_names;
    std::vector<std::string> function_names;
    std::vector<std::string> procedure_names;
    std::vector<std::string> event_names;
    std::vector<std::string> foreign_key_queries;
    std::vector<std::string> queries_on_schema_end;

    volatile bool md_loaded = false;
    bool md_done = false;

    bool has_sql = true;
    bool has_view_sql = true;
    bool has_data = true;

    bool sql_seen = false;
    bool table_sql_done = false;
    bool view_sql_done = false;

    bool ready() const;

    std::string script_name() const;

    std::string metadata_name() const;

    bool should_fetch_metadata_file(const Files &files) const;

    void update_metadata(const std::string &data, Dump_reader *reader);

    void rescan(mysqlshdk::storage::IDirectory *dir, const Files &files,
                Dump_reader *reader, shcore::Thread_pool *pool);

    void check_if_ready();

    void rescan_data(const Files &files, Dump_reader *reader);
  };

  struct Dump_info {
    std::unordered_map<std::string, std::shared_ptr<Schema_info>> schemas;

    std::unique_ptr<std::string> sql;
    std::unique_ptr<std::string> post_sql;
    std::unique_ptr<std::string> users_sql;

    bool has_users = false;

    std::string default_charset;
    std::string binlog_file;
    uint64_t binlog_position = 0;
    std::string gtid_executed;
    bool gtid_executed_inconsistent = false;
    bool tz_utc = true;
    bool mds_compatibility = false;
    bool partial_revokes = false;
    bool create_invisible_pks = false;
    bool table_only = false;
    mysqlshdk::utils::Version server_version;
    mysqlshdk::utils::Version dump_version;
    std::string origin;
    uint64_t bytes_per_chunk = 0;
    std::unordered_map<std::string, uint64_t> chunk_sizes;

    volatile bool md_done = false;

    // total uncompressed bytes of table data the dump contains
    size_t data_size = 0;
    // uncompressed bytes per table
    std::unordered_map<std::string, std::unordered_map<std::string, size_t>>
        table_data_size;
    // total file sizes available in the dump location
    size_t dump_size = 0;

    std::vector<Capability_info> capabilities;

    bool ready() const;

    void rescan(mysqlshdk::storage::IDirectory *dir, const Files &files,
                Dump_reader *reader,
                dump::Progress_thread *progress_thread = nullptr);

    void check_if_ready();

    void parse_done_metadata(mysqlshdk::storage::IDirectory *dir);

   private:
    void rescan_metadata(mysqlshdk::storage::IDirectory *dir,
                         const Files &files, Dump_reader *reader,
                         dump::Progress_thread *progress_thread);

    void rescan_data(const Files &files, Dump_reader *reader);
  };

 private:
  const std::string &override_schema(const std::string &s) const;

  Table_info *find_table(std::string_view schema, std::string_view table,
                         std::string_view context);

  std::unique_ptr<mysqlshdk::storage::IDirectory> m_dir;

  const Load_dump_options &m_options;

  Status m_dump_status = Status::INVALID;
  Dump_info m_contents;
  size_t m_filtered_data_size = 0;

  // Tables and partitions that are ready to be loaded
  std::unordered_set<Table_data_info *> m_tables_with_data;

  // tables which have data to be loaded (possibly partitioned)
  std::atomic<uint64_t> m_tables_to_load{0};

  // tables and partitions which have data to be loaded
  std::atomic<uint64_t> m_tables_and_partitions_to_load{0};

  bool m_dump_has_partitions = false;

  std::atomic<uint64_t> m_metadata_available{0};
  std::atomic<uint64_t> m_metadata_parsed{0};

  // new schema name -> old schema name
  std::optional<std::pair<std::string, std::string>> m_schema_override;

  using Candidate =
      std::unordered_set<Dump_reader::Table_data_info *>::iterator;

  static Candidate schedule_chunk_proportionally(
      const std::unordered_multimap<std::string, size_t> &tables_being_loaded,
      std::unordered_set<Dump_reader::Table_data_info *> *tables_with_data,
      uint64_t max_concurrent_tables);

#ifdef FRIEND_TEST
  FRIEND_TEST(Dump_scheduler, load_scheduler);
#endif
};

}  // namespace mysqlsh

#endif  // MODULES_UTIL_LOAD_DUMP_READER_H_
