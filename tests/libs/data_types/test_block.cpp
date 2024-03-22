#include <cstddef>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/block.hpp"
#include "zkevm_framework/data_types/block_header.hpp"
#include "zkevm_framework/data_types/transaction.hpp"
#include "zkevm_framework/data_types/transaction_receipt.hpp"

using namespace data_types;

TEST(DataTypesBlockTests, SerializeDeserializeBlock) {
    bytes data = {std::byte{0xAA}, std::byte{0xAA}, std::byte{0xAA}};
    Transaction transaction_(1, 2, 3, data, 5);
    transaction_.forceSender({0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                              0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A});

    TransactionReceipt receipt_(Transaction::ContractCreation, 0, 1);

    bytes prev_data = {std::byte{0xBB}, std::byte{0xBB}, std::byte{0xBB}};
    Address prev_author = {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                           0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A};
    BlockHeader prev_hdr(0, 0, 0, 0, 1, 2, 3, prev_data, prev_author, 5);

    bytes curr_data = {std::byte{0xCC}, std::byte{0xCC}, std::byte{0xCC}};
    Address curr_author = {0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
                           0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B};
    BlockHeader curr_hdr(0, 0, 0, 0, 1, 2, 3, curr_data, curr_author, 5);

    Block b({transaction_}, {receipt_}, prev_hdr, curr_hdr);
    bytes blob = b.serialize();

    Block block = Block::deserialize(blob);

    auto transactions = block.getTransactions();
    EXPECT_EQ(transactions.size(), 1);

    auto receipts = block.getReceipts();
    EXPECT_EQ(receipts.size(), 1);

    auto transaction = transactions[0];

    EXPECT_THAT(transaction.sender(),
                testing::ElementsAre(0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                                     0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A));
    ASSERT_TRUE(transaction.isCreation());
    EXPECT_EQ(transaction.value(), 1);
    EXPECT_EQ(transaction.gasPrice(), 2);
    EXPECT_EQ(transaction.gas(), 3);
    EXPECT_THAT(transaction.to(),
                testing::ElementsAre(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00));
    EXPECT_THAT(transaction.from(),
                testing::ElementsAre(0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                                     0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A));
    EXPECT_THAT(transaction.data(),
                testing::ElementsAre(std::byte{0xAA}, std::byte{0xAA}, std::byte{0xAA}));
    EXPECT_EQ(transaction.nonce(), 5);

    auto receipt = receipts[0];

    EXPECT_EQ(receipt.gasUsed(), 1);
    EXPECT_EQ(receipt.stateRoot(), 0);
    EXPECT_EQ(receipt.getType(), Transaction::ContractCreation);

    auto curr_block = block.getCurrentBlock();
    EXPECT_EQ(curr_block.parentHash(), 0);
    EXPECT_EQ(curr_block.stateRoot(), 0);
    EXPECT_EQ(curr_block.transactionsRoot(), 0);
    EXPECT_EQ(curr_block.receiptsRoot(), 0);
    EXPECT_EQ(curr_block.number(), 1);
    EXPECT_EQ(curr_block.gasLimit(), 2);
    EXPECT_EQ(curr_block.gasUsed(), 3);
    EXPECT_THAT(curr_block.extraData(),
                testing::ElementsAre(std::byte{0xCC}, std::byte{0xCC}, std::byte{0xCC}));
    EXPECT_THAT(curr_block.author(),
                testing::ElementsAre(0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
                                     0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B));
    EXPECT_EQ(curr_block.timestamp(), 5);

    auto prev_block = block.getPreviousBlock();
    EXPECT_EQ(prev_block.parentHash(), 0);
    EXPECT_EQ(prev_block.stateRoot(), 0);
    EXPECT_EQ(prev_block.transactionsRoot(), 0);
    EXPECT_EQ(prev_block.receiptsRoot(), 0);
    EXPECT_EQ(prev_block.number(), 1);
    EXPECT_EQ(prev_block.gasLimit(), 2);
    EXPECT_EQ(prev_block.gasUsed(), 3);
    EXPECT_THAT(prev_block.extraData(),
                testing::ElementsAre(std::byte{0xBB}, std::byte{0xBB}, std::byte{0xBB}));
    EXPECT_THAT(prev_block.author(),
                testing::ElementsAre(0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                                     0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A));
    EXPECT_EQ(prev_block.timestamp(), 5);
}
