# Copyright (c) 2022, 2024 Oracle and/or its affiliates.
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

IF(OPENSSL_INCLUDE_DIR)
  INCLUDE_DIRECTORIES(BEFORE ${OPENSSL_INCLUDE_DIR})
ENDIF()

set(aws_SOURCES
  aws_config_file.cc
  aws_credentials.cc
  aws_credentials_provider.cc
  aws_signer.cc
  config_credentials_provider.cc
  env_credentials_provider.cc
  process_credentials_provider.cc
  s3_bucket.cc
  s3_bucket_config.cc
  s3_bucket_options.cc
)

set(aws_LINK_LIBRARIES utils)

if (WIN32)
  list(APPEND aws_LINK_LIBRARIES Crypt32)
endif()

add_convenience_library(aws
  ${aws_SOURCES}
  INCLUDE_DIRECTORIES ${TINYXML2_INCLUDE_DIRECTORIES}
  LINK_LIBRARIES ${aws_LINK_LIBRARIES}
)
