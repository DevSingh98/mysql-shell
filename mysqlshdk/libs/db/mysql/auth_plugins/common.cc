/*
 * Copyright (c) 2022, 2024, Oracle and/or its affiliates.
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

#include "mysqlshdk/libs/db/mysql/auth_plugins/common.h"

#include "mysqlshdk/libs/db/session.h"

namespace mysqlshdk {
namespace db {
namespace mysql {
namespace auth {

void handle_mysql_error(MYSQL *conn) {
  const auto code = mysql_errno(conn);
  throw mysqlshdk::db::Error(mysql_error(conn), code, mysql_sqlstate(conn));
}

struct st_mysql_client_plugin *get_authentication_plugin(MYSQL *conn,
                                                         const char *name) {
  auto plugin =
      mysql_client_find_plugin(conn, name, MYSQL_CLIENT_AUTHENTICATION_PLUGIN);

  if (!plugin) {
    handle_mysql_error(conn);
  }

  return plugin;
}

}  // namespace auth
}  // namespace mysql
}  // namespace db
}  // namespace mysqlshdk
