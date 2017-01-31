/**
 * Copyright (c) 2016 DeepCortex GmbH <legal@eventql.io>
 * Authors:
 *   - Paul Asmuth <paul@eventql.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#pragma once
#include <eventql/util/stdtypes.h>
#include <eventql/util/ieee754.h>
#include <eventql/util/util/binarymessagereader.h>
#include <eventql/util/util/binarymessagewriter.h>
#include <eventql/sql/svalue.h>
#include <eventql/sql/transaction.h>

#include "eventql/eventql.h"

namespace csql {
class VMRegister;

enum kFunctionType {
  FN_PURE,
  FN_AGGREGATE
};

struct SFunction {

  SFunction(
      std::vector<SType> _arg_types,
      SType _return_type,
      void (*_call)(sql_txn* ctx, int argc, void** argv, VMRegister* out),
      bool _has_side_effects = false);

  SFunction(
      std::vector<SType> _arg_types,
      SType _return_type,
      size_t _scratch_size,
      void (*_accumulate)(sql_txn*, void* scratch, int argc, void** argv),
      void (*_get)(sql_txn*, void* scratch, VMRegister* out),
      void (*_reset)(sql_txn*, void* scratch),
      void (*_init)(sql_txn*, void* scratch),
      void (*_free)(sql_txn*, void* scratch),
      void (*_merge)(sql_txn*, void* scratch, const void* other),
      void (*_savestate)(sql_txn*, void* scratch, OutputStream* os),
      void (*_loadstate)(sql_txn*, void* scratch, InputStream* is));

  kFunctionType type;
  std::vector<SType> arg_types;
  SType return_type;
  bool has_side_effects;
  size_t scratch_size;

  struct VTable {
    void (*call)(sql_txn* ctx, int argc, void** argv, VMRegister* out);
    void (*accumulate)(sql_txn*, void* scratch, int argc, void** argv);
    void (*get)(sql_txn*, void* scratch, VMRegister* out);
    void (*reset)(sql_txn*, void* scratch);
    void (*init)(sql_txn*, void* scratch);
    void (*free)(sql_txn*, void* scratch);
    void (*merge)(sql_txn*, void* scratch, const void* other);
    void (*savestate)(sql_txn*, void* scratch, OutputStream* os);
    void (*loadstate)(sql_txn*, void* scratch, InputStream* is);
  } vtable;
};

} // namespace csql
