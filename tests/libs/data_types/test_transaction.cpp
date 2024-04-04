#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

using namespace data_types;

TEST(DataTypesTransactionTests, SerializeDeserializeTransaction) {
    Address addr = {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A};
    bytes data = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
    Transaction transaction(Transaction::Type::ContractCreation, 1, 2, addr, 3, 4, data, addr);

    bytes blob = transaction.serialize();

    Transaction result = Transaction::deserialize(blob);

    EXPECT_EQ(result.m_type, Transaction::Type::ContractCreation);
    EXPECT_EQ(result.m_nonce, 1);
    EXPECT_EQ(result.m_value, 2);
    EXPECT_EQ(result.m_gasPrice, 3);
    EXPECT_EQ(result.m_gas, 4);
    EXPECT_THAT(result.m_receiveAddress, testing::ElementsAreArray(addr));
    EXPECT_THAT(result.m_sender, testing::ElementsAreArray(addr));
    EXPECT_THAT(result.m_data, testing::ElementsAreArray(data));
}
