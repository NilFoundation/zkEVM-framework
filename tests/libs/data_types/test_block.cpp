#include <cstddef>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "zkevm_framework/data_types/account_block.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/block.hpp"
#include "zkevm_framework/data_types/block_header.hpp"
#include "zkevm_framework/data_types/message.hpp"
#include "zkevm_framework/data_types/mpt.hpp"

using namespace data_types;

TEST(DataTypesBlockTests, SerializeDeserializeBlock) {
    bytes data = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
    size_t value = 42;
    Address addr = {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A};
    Transaction transaction(0, Transaction::Type::ContractCreation, 1, 2, addr, 3, 4, data, addr);

    BlockHeader prev_hdr(0, 1, 2, 3, addr, 4, 1, 55, 55, data, 5);
    BlockHeader curr_hdr(0, 1, 2, 3, addr, 4, 1, 55, 55, data, 5);
    MPTNode<Transaction> transactions = {};
    transactions.push_back(transaction);
    AccountBlock account_block(addr, transactions);
    MPTNode<AccountBlock> account_blocks = {};
    account_blocks.push_back(account_block);
    CommonMsgInfo msg_info(addr, addr, value);
    MPTNode<InMsg> in_msgs;
    in_msgs.push_back({msg_info, transaction});
    MPTNode<OutMsg> out_msgs;
    out_msgs.push_back({msg_info, transaction});

    Block b(account_blocks, in_msgs, out_msgs, prev_hdr, curr_hdr);
    bytes blob = b.serialize();

    Block block = Block::deserialize(blob);

    EXPECT_EQ(block.m_accountBlocks.size(), 1);
    AccountBlock acc_block = block.m_accountBlocks[0];

    EXPECT_EQ(acc_block.m_accountAddress, addr);
    EXPECT_EQ(acc_block.m_transactions.size(), 1);
    Transaction t = acc_block.m_transactions[0];
    EXPECT_EQ(t.m_id, 0);
    EXPECT_EQ(t.m_type, Transaction::Type::ContractCreation);
    EXPECT_EQ(t.m_nonce, 1);
    EXPECT_EQ(t.m_value, 2);
    EXPECT_EQ(t.m_gasPrice, 3);
    EXPECT_EQ(t.m_gas, 4);
    EXPECT_THAT(t.m_receiveAddress, testing::ElementsAreArray(addr));
    EXPECT_THAT(t.m_sender, testing::ElementsAreArray(addr));
    EXPECT_THAT(t.m_data, testing::ElementsAreArray(data));

    EXPECT_EQ(block.m_currentBlock.m_parentHash, 0);
    EXPECT_EQ(block.m_currentBlock.m_number, 1);
    EXPECT_EQ(block.m_currentBlock.m_gasLimit, 2);
    EXPECT_EQ(block.m_currentBlock.m_gasUsed, 3);
    EXPECT_THAT(block.m_currentBlock.m_coinbase, testing::ElementsAreArray(addr));
    EXPECT_EQ(block.m_currentBlock.m_prevrandao, 4);
    EXPECT_EQ(block.m_currentBlock.m_chain_id, 1);
    EXPECT_EQ(block.m_currentBlock.m_basefee, 55);
    EXPECT_EQ(block.m_currentBlock.m_blob_basefee, 55);
    EXPECT_THAT(block.m_currentBlock.m_extraData, testing::ElementsAreArray(data));
    EXPECT_EQ(block.m_currentBlock.m_timestamp, 5);

    EXPECT_EQ(block.m_previousBlock.m_parentHash, 0);
    EXPECT_EQ(block.m_previousBlock.m_number, 1);
    EXPECT_EQ(block.m_previousBlock.m_gasLimit, 2);
    EXPECT_EQ(block.m_previousBlock.m_gasUsed, 3);
    EXPECT_THAT(block.m_previousBlock.m_coinbase, testing::ElementsAreArray(addr));
    EXPECT_EQ(block.m_previousBlock.m_prevrandao, 4);
    EXPECT_EQ(block.m_previousBlock.m_chain_id, 1);
    EXPECT_EQ(block.m_previousBlock.m_basefee, 55);
    EXPECT_EQ(block.m_previousBlock.m_blob_basefee, 55);
    EXPECT_THAT(block.m_previousBlock.m_extraData, testing::ElementsAreArray(data));
    EXPECT_EQ(block.m_previousBlock.m_timestamp, 5);

    EXPECT_EQ(block.m_inputMsgs[0].m_info.m_src, addr);
    EXPECT_EQ(block.m_inputMsgs[0].m_info.m_dst, addr);
    EXPECT_EQ(block.m_inputMsgs[0].m_info.m_value, value);
    Transaction msg_t = block.m_inputMsgs[0].m_transaction;
    EXPECT_EQ(msg_t.m_id, 0);
    EXPECT_EQ(msg_t.m_type, Transaction::Type::ContractCreation);
    EXPECT_EQ(msg_t.m_nonce, 1);
    EXPECT_EQ(msg_t.m_value, 2);
    EXPECT_EQ(msg_t.m_gasPrice, 3);
    EXPECT_EQ(msg_t.m_gas, 4);
    EXPECT_THAT(msg_t.m_receiveAddress, testing::ElementsAreArray(addr));
    EXPECT_THAT(msg_t.m_sender, testing::ElementsAreArray(addr));
    EXPECT_THAT(msg_t.m_data, testing::ElementsAreArray(data));

    EXPECT_EQ(block.m_outputMsgs[0].m_info.m_src, addr);
    EXPECT_EQ(block.m_outputMsgs[0].m_info.m_dst, addr);
    EXPECT_EQ(block.m_outputMsgs[0].m_info.m_value, value);
    Transaction out_msg_t = block.m_outputMsgs[0].m_transaction;
    EXPECT_EQ(out_msg_t.m_id, 0);
    EXPECT_EQ(out_msg_t.m_type, Transaction::Type::ContractCreation);
    EXPECT_EQ(out_msg_t.m_nonce, 1);
    EXPECT_EQ(out_msg_t.m_value, 2);
    EXPECT_EQ(out_msg_t.m_gasPrice, 3);
    EXPECT_EQ(out_msg_t.m_gas, 4);
    EXPECT_THAT(out_msg_t.m_receiveAddress, testing::ElementsAreArray(addr));
    EXPECT_THAT(out_msg_t.m_sender, testing::ElementsAreArray(addr));
    EXPECT_THAT(out_msg_t.m_data, testing::ElementsAreArray(data));
}
