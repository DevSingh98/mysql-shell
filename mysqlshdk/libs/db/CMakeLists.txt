# Copyright (c) 2014, 2024, Oracle and/or its affiliates.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0,
# as published by the Free Software Foundation.
#
# This program is designed to work with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms,
# as designated in a particular file or component or in included license
# documentation.  The authors of MySQL hereby grant you an additional
# permission to link the program and your derivative works with the
# separately licensed software that they have either included with
# the program or referenced in the documentation.
#
# This program is distributed in the hope that it will be useful,  but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
# the GNU General Public License, version 2.0, for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

set(db_SOURCE
    session.h
    session.cc
    result.h
    row.h
    column.cc
    charset.cc
    uri_parser.cc
    uri_encoder.cc
    connection_options.cc
    ssl_options.cc
    utils_connection.cc
    utils_error.cc
    row.cc
    row_copy.cc
    mutable_result.cc
    uri_common.cc
    generic_uri.cc
    file_uri.cc
    utils/diff.cc
    utils/utils.cc
    mysql/session.cc
    mysql/result.cc
    mysql/row.cc
    mysql/auth_plugins/mysql_event_handler_plugin.cc
    mysql/auth_plugins/common.cc
    mysql/auth_plugins/fido.cc
    mysql/auth_plugins/oci.cc
    mysqlx/xsession.cc
    mysqlx/xresult.cc
    mysqlx/xrow.cc
    mysqlx/orderby_parser.cc
    mysqlx/tokenizer.cc
    mysqlx/expr_parser.cc
    mysqlx/proj_parser.cc
    replay/mysqlx.cc
    replay/setup.cc
    replay/recorder.cc
    replay/replayer.cc
    replay/trace.cc
)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
  SET(mysqlsh_tests_SRC
      ${mysqlsh_tests_SRC}
      session_recorder.cc
  )
ENDIF()

add_convenience_library(db ${db_SOURCE})
ADD_STAN_TARGET(db ${db_SOURCE})

target_link_libraries(db utils shellssh)
