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

#include "modules/util/dump/export_table_options.h"

#include "mysqlshdk/include/scripting/type_info/custom.h"
#include "mysqlshdk/include/scripting/type_info/generic.h"
#include "mysqlshdk/libs/utils/utils_general.h"
#include "mysqlshdk/libs/utils/utils_sqlstring.h"

namespace mysqlsh {
namespace dump {

Export_table_options::Export_table_options()
    : m_blob_storage_options{
          mysqlshdk::azure::Blob_storage_options::Operation::WRITE} {
  // calling this in the constructor sets the default value
  set_compression(mysqlshdk::storage::Compression::NONE);
}

const shcore::Option_pack_def<Export_table_options>
    &Export_table_options::options() {
  static const auto opts =
      shcore::Option_pack_def<Export_table_options>()
          .include<Dump_options>()
          .optional("where", &Export_table_options::m_where)
          .optional("partitions", &Export_table_options::m_partitions)
          .include(&Export_table_options::m_oci_bucket_options)
          .include(&Export_table_options::m_s3_bucket_options)
          .include(&Export_table_options::m_blob_storage_options)
          .on_done(&Export_table_options::on_unpacked_options)
          .on_log(&Export_table_options::on_log_options);

  return opts;
}

void Export_table_options::on_unpacked_options() {
  m_s3_bucket_options.throw_on_conflict(m_oci_bucket_options);
  m_s3_bucket_options.throw_on_conflict(m_blob_storage_options);
  m_blob_storage_options.throw_on_conflict(m_oci_bucket_options);

  if (m_oci_bucket_options) {
    set_storage_config(m_oci_bucket_options.config());
  }

  if (m_s3_bucket_options) {
    set_storage_config(m_s3_bucket_options.config());
  }

  if (m_blob_storage_options) {
    set_storage_config(m_blob_storage_options.config());
  }
}

void Export_table_options::set_table(const std::string &schema_table) {
  try {
    shcore::split_schema_and_table(schema_table, &m_schema, &m_table);
    on_set_schema();
  } catch (const std::runtime_error &e) {
    throw std::invalid_argument("Failed to parse table to be exported '" +
                                schema_table + "': " + e.what());
  }
}

void Export_table_options::on_set_session(
    const std::shared_ptr<mysqlshdk::db::ISession> &session) {
  if (m_schema.empty()) {
    const auto result = session->query("SELECT SCHEMA();");

    if (const auto row = result->fetch_one()) {
      if (!row->is_null(0)) {
        m_schema = row->get_string(0);
      }
    }

    on_set_schema();
  }
}

void Export_table_options::validate_options() const {
  if (schema().empty()) {
    throw std::invalid_argument(
        "The table was given without a schema and there is no active schema "
        "on the current session, unable to deduce which table to export.");
  }

  if (!exists(schema(), table())) {
    throw std::invalid_argument(
        "The requested table " + shcore::quote_identifier(schema()) + "." +
        shcore::quote_identifier(table()) + " was not found in the database.");
  }
}

void Export_table_options::on_set_schema() {
  if (!schema().empty()) {
    filters().schemas().include(schema());
    filters().tables().include(schema(), table());

    set_where_clause(schema(), table(), m_where);
    set_partitions(schema(), table(), m_partitions);
  }
}

}  // namespace dump
}  // namespace mysqlsh
