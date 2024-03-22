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

    EXPECT_EQ(result.gasUsed(), 1);
    EXPECT_EQ(result.stateRoot(), 0);
    EXPECT_EQ(result.getType(), Transaction::ContractCreation);
}
