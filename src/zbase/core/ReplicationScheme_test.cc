/**
 * Copyright (c) 2015 - The zScale Authors <legal@zscale.io>
 *   All Rights Reserved.
 *
 * This file is CONFIDENTIAL -- Distribution or duplication of this material or
 * the information contained herein is strictly forbidden unless prior written
 * permission is obtained.
 */
#include "stx/stdtypes.h"
#include "stx/test/unittest.h"
#include "zbase/core/ReplicationScheme.h"

using namespace stx;
using namespace zbase;

UNIT_TEST(ReplicationSchemeTest);

TEST_CASE(ReplicationSchemeTest, TestHostAssignmentWithTwoNodes, [] () {
  ClusterConfig cc;
  cc.set_dht_num_copies(3);

  {
    auto node = cc.add_dht_nodes();
    node->set_status(DHTNODE_LIVE);
    node->set_name("nodeA");
    node->set_addr("1.1.1.1:1234");
    *node->add_sha1_tokens() = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    *node->add_sha1_tokens() = "6666666666666666666666666666666666666666";
  }

  {
    auto node = cc.add_dht_nodes();
    node->set_status(DHTNODE_LIVE);
    node->set_name("nodeB");
    node->set_addr("2.2.2.2:1234");
    *node->add_sha1_tokens() = "3333333333333333333333333333333333333333";
  }


  DHTReplicationScheme rscheme(cc);

  {
    auto replicas = rscheme.replicasFor(
        SHA1Hash::fromHexString("1111111111111111111111111111111111111111"));

    EXPECT_EQ(replicas.size(), 2);
    EXPECT_EQ(
        replicas[0].unique_id.toString(),
        "3333333333333333333333333333333333333333");
    EXPECT_EQ(replicas[0].addr.ipAndPort(), "2.2.2.2:1234");
    EXPECT_EQ(
        replicas[1].unique_id.toString(),
        "6666666666666666666666666666666666666666");
    EXPECT_EQ(replicas[1].addr.ipAndPort(), "1.1.1.1:1234");
  }

  {
    auto replicas = rscheme.replicasFor(
        SHA1Hash::fromHexString("8888888888888888888888888888888888888888"));

    EXPECT_EQ(replicas.size(), 2);
    EXPECT_EQ(
        replicas[0].unique_id.toString(),
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    EXPECT_EQ(replicas[0].addr.ipAndPort(), "1.1.1.1:1234");
    EXPECT_EQ(
        replicas[1].unique_id.toString(),
        "3333333333333333333333333333333333333333");
    EXPECT_EQ(replicas[1].addr.ipAndPort(), "2.2.2.2:1234");
  }

  {
    auto replicas = rscheme.replicasFor(
        SHA1Hash::fromHexString("dddddddddddddddddddddddddddddddddddddddd"));

    EXPECT_EQ(replicas.size(), 2);
    EXPECT_EQ(
        replicas[0].unique_id.toString(),
        "3333333333333333333333333333333333333333");
    EXPECT_EQ(replicas[0].addr.ipAndPort(), "2.2.2.2:1234");
    EXPECT_EQ(
        replicas[1].unique_id.toString(),
        "6666666666666666666666666666666666666666");
    EXPECT_EQ(replicas[1].addr.ipAndPort(), "1.1.1.1:1234");
  }
});


