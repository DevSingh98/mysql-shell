# Copyright (c) 2017, 2025, Oracle and/or its affiliates.
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

include_directories(SYSTEM
    ${CMAKE_SOURCE_DIR}/common/uuid/include
    ${CMAKE_SOURCE_DIR}/ext/rapidjson/include)

include_directories(BEFORE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/mysqlshdk/libs
    ${CMAKE_SOURCE_DIR}/mysqlshdk/include
    ${CMAKE_BINARY_DIR}/mysqlshdk/include)

ADD_SUBDIRECTORY(libs/tinyxml2)
ADD_SUBDIRECTORY(libs/parser)
ADD_SUBDIRECTORY(libs/utils)
ADD_SUBDIRECTORY(libs/db)
ADD_SUBDIRECTORY(libs/mysql)
ADD_SUBDIRECTORY(libs/textui)
ADD_SUBDIRECTORY(libs/config)
ADD_SUBDIRECTORY(libs/secret-store-api)
ADD_SUBDIRECTORY(libs/rest)
ADD_SUBDIRECTORY(libs/storage)
ADD_SUBDIRECTORY(libs/oci)
ADD_SUBDIRECTORY(libs/azure)
ADD_SUBDIRECTORY(libs/ssh)
ADD_SUBDIRECTORY(libs/aws)
ADD_SUBDIRECTORY(scripting)
ADD_SUBDIRECTORY(shellcore)


#TODO: ADD_VERSION_INFO
#merge_libraries(mysqlshdk SHARED
#    utils
#    db
#    scripting
#    shellcore)

merge_convenience_libraries(mysqlshdk-static
    utils
    db
    shellssh
    mysql
    config
    mysql-secret-store-api
    rest
    textui
    scripting
    shellcore
    storage
    parser
    oci
    azure
    aws
    tinyxml2 SKIP_INSTALL)

target_link_libraries(mysqlshdk-static PUBLIC
          ${PYTHON_LIBRARIES}
          ${MYSQL_LIBRARIES}
          ${MYSQLX_LIBRARIES}
          ${PROTOBUF_LIBRARIES}
          ${LZ4_LIBRARY}
          ${SSL_LIBRARIES}
          ${ANTLR4_LIBRARIES})

IF(HAVE_JIT_EXECUTOR)
    target_link_libraries(mysqlshdk-static PUBLIC ${JIT_EXECUTOR_LIBRARY})
ENDIF()
