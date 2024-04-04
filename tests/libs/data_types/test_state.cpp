#include <unordered_map>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "zkevm_framework/data_types/account.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/message.hpp"
#include "zkevm_framework/data_types/state.hpp"

using namespace data_types;

TEST(DataTypesStateTests, SerializeDeserializeState) {
    Address src = {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                   0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A};
    Address dst = {0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
                   0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B};
    size_t value = 42;
    CommonMsgInfo info(src, dst, value);
    OutMsg msg(info);
    MPTNode<OutMsg> out_msgs = {};
    out_msgs.push_back(msg);
    Account acc(1, 2, EVMC_ACCESS_COLD);
    MPTNode<Account> accounts = {};
    accounts.push_back(acc);

    State s(0, 1, 2, 3, 4, 5, true, 7, 8, 9, out_msgs, accounts);
    bytes blob = s.serialize();

    State result = State::deserialize(blob);

    EXPECT_EQ(result.m_globalId, 0);
    EXPECT_EQ(result.m_shardId, 1);
    EXPECT_EQ(result.m_seqNo, 2);
    EXPECT_EQ(result.m_vertSeqNo, 3);
    EXPECT_EQ(result.m_genLt, 4);
    EXPECT_EQ(result.m_minRefMcSeqno, 5);
    EXPECT_EQ(result.m_beforeSplit, true);
    EXPECT_EQ(result.m_genUtime, 7);
    EXPECT_EQ(result.m_totalBalance, 8);
    EXPECT_EQ(result.m_totalValidatorFees, 9);

    EXPECT_EQ(result.m_outMsg.size(), 1);
    EXPECT_EQ(result.m_outMsg[0].m_info.m_src, src);
    EXPECT_EQ(result.m_outMsg[0].m_info.m_dst, dst);
    EXPECT_EQ(result.m_outMsg[0].m_info.m_value, value);

    EXPECT_EQ(result.m_accounts.size(), 1);
    EXPECT_EQ(result.m_accounts[0].m_accessStatus, EVMC_ACCESS_COLD);
    EXPECT_EQ(result.m_accounts[0].m_balance, 2);
    EXPECT_EQ(result.m_accounts[0].m_nonce, 1);
}
