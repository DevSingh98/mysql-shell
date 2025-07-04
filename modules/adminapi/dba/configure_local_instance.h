/*
 * Copyright (c) 2017, 2024, Oracle and/or its affiliates.
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

#ifndef MODULES_ADMINAPI_DBA_CONFIGURE_LOCAL_INSTANCE_H_
#define MODULES_ADMINAPI_DBA_CONFIGURE_LOCAL_INSTANCE_H_

#include <map>
#include <memory>
#include <string>

#include "modules/adminapi/common/common.h"
#include "modules/adminapi/common/instance_pool.h"
#include "modules/adminapi/common/provisioning_interface.h"
#include "modules/adminapi/dba/configure_instance.h"
#include "scripting/lang_base.h"

namespace mysqlsh {
namespace dba {

class Configure_local_instance final : public Configure_instance {
 public:
  Configure_local_instance(
      const std::shared_ptr<mysqlsh::dba::Instance> &target_instance,
      const Configure_instance_options &options, Cluster_type purpose);

  void prepare() override;
  shcore::Value execute() override;

 private:
  TargetType::Type m_instance_type;
};

}  // namespace dba
}  // namespace mysqlsh

#endif  // MODULES_ADMINAPI_DBA_CONFIGURE_LOCAL_INSTANCE_H_
