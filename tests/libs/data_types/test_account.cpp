#include <sstream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "zkevm_framework/data_types/account.hpp"
#include "zkevm_framework/data_types/base.hpp"

using namespace data_types;

TEST(DataTypesAccountTests, SerializeDeserializeAccount) {
    Account acc(1, 2, EVMC_ACCESS_COLD);
    bytes blob = *acc.serialize();

    Account result = *Account::deserialize(blob);

    EXPECT_EQ(result.m_accessStatus, EVMC_ACCESS_COLD);
    EXPECT_EQ(result.m_balance, 2);
    EXPECT_EQ(result.m_nonce, 1);
}

TEST(DataTypesAccountTests, StreamSerializeDeserializeAccount) {
    Account acc(1, 2, EVMC_ACCESS_COLD);
    std::stringstream ss;
    acc.serialize(ss);

    std::optional<Account> opt_result = *Account::deserialize(ss);
    EXPECT_TRUE(opt_result.has_value());
    Account result = opt_result.value();

    EXPECT_EQ(result.m_accessStatus, EVMC_ACCESS_COLD);
    EXPECT_EQ(result.m_balance, 2);
    EXPECT_EQ(result.m_nonce, 1);
}
