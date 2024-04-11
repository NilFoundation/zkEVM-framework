#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "zkevm_framework/data_types/account_block.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/mpt.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

using namespace data_types;

TEST(DataTypesAccountTests, SerializeDeserializeAccount) {
    Address addr = {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A};
    bytes data = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
    Transaction transaction(0, Transaction::Type::ContractCreation, 1, 2, addr, 3, 4, data, addr);
    MPTNode<Transaction> transactions = {};
    transactions.push_back(transaction);
    AccountBlock acc(addr, transactions);
    bytes blob = acc.serialize();

    AccountBlock result = AccountBlock::deserialize(blob);

    EXPECT_EQ(result.m_accountAddress, addr);
    EXPECT_EQ(result.m_transactions.size(), 1);

    Transaction t = result.m_transactions[0];
    EXPECT_EQ(t.m_id, 0);
    EXPECT_EQ(t.m_type, Transaction::Type::ContractCreation);
    EXPECT_EQ(t.m_nonce, 1);
    EXPECT_EQ(t.m_value, 2);
    EXPECT_EQ(t.m_gasPrice, 3);
    EXPECT_EQ(t.m_gas, 4);
    EXPECT_THAT(t.m_receiveAddress, testing::ElementsAreArray(addr));
    EXPECT_THAT(t.m_sender, testing::ElementsAreArray(addr));
    EXPECT_THAT(t.m_data, testing::ElementsAreArray(data));
}
