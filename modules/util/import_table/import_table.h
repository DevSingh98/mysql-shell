/*
 * Copyright (c) 2018, 2024, Oracle and/or its affiliates.
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

#ifndef MODULES_UTIL_IMPORT_TABLE_IMPORT_TABLE_H_
#define MODULES_UTIL_IMPORT_TABLE_IMPORT_TABLE_H_

#include <atomic>
#include <exception>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "modules/util/dump/progress_thread.h"
#include "modules/util/import_table/chunk_file.h"
#include "modules/util/import_table/import_table_options.h"
#include "mysqlshdk/include/shellcore/scoped_contexts.h"
#include "mysqlshdk/libs/textui/text_progress.h"
#include "mysqlshdk/libs/utils/profiling.h"
#include "mysqlshdk/libs/utils/synchronized_queue.h"

namespace mysqlsh {
namespace import_table {

struct Stats {
  std::atomic<size_t> total_records{0};
  std::atomic<size_t> total_deleted{0};
  std::atomic<size_t> total_skipped{0};
  std::atomic<size_t> total_warnings{0};
  // total number of uncompressed bytes processed
  std::atomic<size_t> total_data_bytes{0};
  // total number of physical bytes processed
  std::atomic<size_t> total_file_bytes{0};
  std::atomic<size_t> total_files_processed{0};

  std::string to_string() const {
    return std::string{"Records: " + std::to_string(total_records) +
                       "  Deleted: " + std::to_string(total_deleted) +
                       "  Skipped: " + std::to_string(total_skipped) +
                       "  Warnings: " + std::to_string(total_warnings)};
  }
};

class Import_table final {
 public:
  Import_table() = delete;
  explicit Import_table(const Import_table_options &options);
  Import_table(const Import_table &other) = delete;
  Import_table(Import_table &&other) = delete;

  Import_table &operator=(const Import_table &other) = delete;
  Import_table &operator=(Import_table &&other) = delete;

  ~Import_table();

  void interrupt(volatile bool *interrupt) { m_interrupt = interrupt; }

  void import();
  bool any_exception();
  void rethrow_exceptions();

  std::string import_summary() const;
  std::string rows_affected_info();

 private:
  void spawn_workers();
  void join_workers();
  void chunk_file();
  void build_queue();
  void progress_setup();
  void progress_shutdown();

  std::atomic<size_t> m_prog_sent_bytes{0};
  std::atomic<size_t> m_prog_file_bytes{0};
  size_t m_total_file_size = 0;
  bool m_has_compressed_files = false;

  shcore::Synchronized_queue<File_import_info> m_range_queue;

  const Import_table_options &m_opt;
  Stats m_stats;

  volatile bool *m_interrupt;
  std::vector<std::thread> m_threads;
  std::vector<std::exception_ptr> m_thread_exception;

  // Store messages from errors that are not critical for import procedure, but
  // required for setting non-zero exit code.
  std::vector<std::string> noncritical_errors;

  // progress thread needs to be placed after any of the fields it uses, in
  // order to ensure that it is destroyed (and stopped) before any of those
  // fields
  dump::Progress_thread m_progress_thread;
};

}  // namespace import_table
}  // namespace mysqlsh

#endif  // MODULES_UTIL_IMPORT_TABLE_IMPORT_TABLE_H_
