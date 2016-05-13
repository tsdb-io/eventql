/**
 * This file is part of the "tsdb" project
 *   Copyright (c) 2015 Paul Asmuth, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <eventql/util/stdtypes.h>
#include <eventql/util/random.h>
#include <eventql/util/option.h>
#include <eventql/util/mdb/MDB.h>
#include <eventql/core/TableConfig.pb.h>
#include <eventql/core/Partition.h>
#include <eventql/core/LazyPartition.h>
#include <eventql/core/TSDBNodeConfig.pb.h>
#include <eventql/core/TSDBTableInfo.h>
#include <eventql/core/PartitionInfo.pb.h>
#include <eventql/core/PartitionChangeNotification.h>
#include <eventql/core/RecordEnvelope.pb.h>
#include <eventql/core/ServerConfig.h>
#include <eventql/z1stats.h>

using namespace stx;

namespace zbase {

class PartitionMap {
public:

  PartitionMap(ServerConfig* cfg);

  void configureTable(const TableDefinition& config);
  void open();

  Option<RefPtr<Table>> findTable(
      const String& tsdb_namespace,
      const String& table_name) const;

  void listTables(
      const String& tsdb_namespace,
      Function<void (const TSDBTableInfo& table)> fn) const;

  void listTablesReverse(
      const String& tsdb_namespace,
      Function<void (const TSDBTableInfo& table)> fn) const;

  Option<TSDBTableInfo> tableInfo(
      const String& tsdb_namespace,
      const String& table_key) const;

  Option<RefPtr<Partition>> findPartition(
      const String& tsdb_namespace,
      const String& table_name,
      const SHA1Hash& partition_key);

  RefPtr<Partition> findOrCreatePartition(
      const String& tsdb_namespace,
      const String& table_name,
      const SHA1Hash& partition_key);

  void subscribeToPartitionChanges(PartitionChangeCallbackFn fn);

  void publishPartitionChange(RefPtr<PartitionChangeNotification> change);

  /**
   * Attempt to drop a partition from the local host (i.e. delete the partition
   * data).
   *
   * The partition will only be deleted if:
   *
   *   - the partition was replicated to and acknowledged by at least N other
   *     hosts. I.e. we can be sure at least N other copies of the data exist
   *     in the cluster. N is the min. number of copies as configured by the 
   *     replication schema.
   *
   *   - the local host does not appear in the preference list for this
   *     partition, i.e. the local host should not store this partition (this
   *     condition is only met after a cluster rebalance or after a failover
   *     store into a non-owning replica)
   *
   * Note that this method has an inherent race condition: A caller would
   * usually check that the partition is fully replicated and not locally owned
   * before calling this method. However there is no way to ensure that no
   * intermittent stores happen in the partition (in between checking the
   * precondition and calling this method). So the method returns a boolean
   * indicating success or failure. 
   *
   * The method will return true iff the partition was succesfully dropped. A
   * false return value indicates that one of the two conditions above weren't
   * met.
   */
  bool dropLocalPartition(
      const String& tsdb_namespace,
      const String& table_name,
      const SHA1Hash& partition_key);

protected:

  Option<RefPtr<Table>> findTableWithLock(
      const String& tsdb_namespace,
      const String& table_name) const;

  void loadPartitions(const Vector<PartitionKey>& partitions);

  ServerConfig* cfg_;
  RefPtr<mdb::MDB> db_;

  mutable std::mutex mutex_;
  OrderedMap<String, RefPtr<Table>> tables_;
  HashMap<String, ScopedPtr<LazyPartition>> partitions_;
  Vector<PartitionChangeCallbackFn> callbacks_;
};

} // namespace tdsb
