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

#include "mysqlshdk/libs/mysql/lock_service.h"

#include <mysqld_error.h>

#include "mysqlshdk/libs/utils/utils_sqlstring.h"

namespace mysqlshdk {
namespace mysql {

std::string to_string(const Lock_mode mode) {
  switch (mode) {
    case Lock_mode::SHARED:
      return "SHARED";
    case Lock_mode::EXCLUSIVE:
      return "EXCLUSIVE";
    case Lock_mode::NONE:
      return "NONE";
    default:
      throw std::logic_error("Unexpected lock mode.");
  }
}

void install_lock_service(mysqlshdk::mysql::IInstance *instance) {
  std::string plugin_lib =
      "locking_service" + instance->get_plugin_library_extension();

  mysqlshdk::mysql::Suppress_binary_log nobinlog(instance);

  auto install = [&plugin_lib, &instance](const std::string &func_name) {
    try {
      log_debug("Installing '%s' lock service...", func_name.c_str());
      instance->executef(
          "CREATE FUNCTION ! RETURNS INT "
          "SONAME /*(*/ ? /*)*/",
          func_name, plugin_lib);
    } catch (const shcore::Error &err) {
      // Ignore error if it is because the lock service already exists.
      if (err.code() == ER_UDF_EXISTS) {
        log_debug("Lock service '%s' already installed (skipping error).",
                  func_name.c_str());
      } else {
        throw;
      }
    }
  };

  install("service_get_read_locks");
  install("service_get_write_locks");
  install("service_release_locks");
}

bool has_lock_service(const mysqlshdk::mysql::IInstance &instance) {
  std::string plugin_lib =
      "locking_service" + instance.get_plugin_library_extension();

  // Check existence of lock service functions from mysql.func.
  // NOTE: Not using performance_schema.user_defined_functions because it does
  //       not exist for 5.7 servers.
  int64_t func_count = instance
                           .query(
                               "SELECT COUNT(*) "
                               "FROM mysql.func "
                               "WHERE dl = /*(*/ '" +
                               plugin_lib +
                               "' /*(*/ "
                               "AND name IN ('service_get_read_locks', "
                               "'service_get_write_locks', "
                               "'service_release_locks')")
                           ->fetch_one()
                           ->get_int(0);

  // Return true only if the 3 lock service functions are available.
  return (func_count == 3);
}

void uninstall_lock_service(mysqlshdk::mysql::IInstance *instance) {
  mysqlshdk::mysql::Suppress_binary_log nobinlog(instance);

  instance->execute("DROP FUNCTION IF EXISTS service_get_read_locks");
  instance->execute("DROP FUNCTION IF EXISTS service_get_write_locks");
  instance->execute("DROP FUNCTION IF EXISTS service_release_locks");
}

void get_lock(const mysqlshdk::mysql::IInstance &instance,
              const std::string &name_space, const std::string &lock_name,
              Lock_mode lock_mode, unsigned int timeout) {
  shcore::sqlstring stmt;
  if (lock_mode == Lock_mode::EXCLUSIVE) {
    stmt = shcore::sqlstring("SELECT service_get_write_locks(?, ?, ?)", 0);
  } else if (lock_mode == Lock_mode::SHARED) {
    stmt = shcore::sqlstring("SELECT service_get_read_locks(?, ?, ?)", 0);
  } else {
    throw shcore::Exception::logic_error("Invalid lock mode for get_lock(): " +
                                         to_string(lock_mode));
  }
  stmt << name_space;
  stmt << lock_name;
  stmt << timeout;
  stmt.done();

  // NOTE: Need to fetch the results (row) from the query otherwise the  error
  // "Service lock wait timeout exceeded." is not thrown.
  instance.query(stmt)->fetch_one();
}

void release_lock(const mysqlshdk::mysql::IInstance &instance,
                  const std::string &name_space) {
  shcore::sqlstring stmt =
      shcore::sqlstring("SELECT service_release_locks(?)", 0);
  stmt << name_space;
  stmt.done();
  instance.query(stmt)->fetch_one();
}

}  // namespace mysql
}  // namespace mysqlshdk
