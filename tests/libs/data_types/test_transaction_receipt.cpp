#include <sstream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/transaction.hpp"
#include "zkevm_framework/data_types/transaction_receipt.hpp"

using namespace data_types;
using namespace testing;

TEST(DataTypesTransactionReceiptTests, SerializeDeserializeTransactionReceipt) {
    TransactionReceipt receipt(Transaction::ContractCreation, 0, 1);
    bytes blob = receipt.serialize();

    TransactionReceipt result = TransactionReceipt::deserialize(blob);

    EXPECT_EQ(result.m_gasUsed, 1);
    EXPECT_EQ(result.m_stateRoot, 0);
    EXPECT_EQ(result.m_type, Transaction::ContractCreation);
}

TEST(DataTypesTransactionReceiptTests, StreamSerializeDeserializeTransactionReceipt) {
    TransactionReceipt receipt(Transaction::ContractCreation, 0, 1);
    std::stringstream ss;
    receipt.serialize(ss);

    std::optional<TransactionReceipt> opt_result = TransactionReceipt::deserialize(ss);
    EXPECT_TRUE(opt_result.has_value());
    TransactionReceipt result = opt_result.value();

    EXPECT_EQ(result.m_gasUsed, 1);
    EXPECT_EQ(result.m_stateRoot, 0);
    EXPECT_EQ(result.m_type, Transaction::ContractCreation);
}
