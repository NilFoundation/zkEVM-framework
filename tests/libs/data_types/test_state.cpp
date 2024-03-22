#include <unordered_map>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "zkevm_framework/data_types/account.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/state.hpp"

using namespace data_types;

TEST(DataTypesStateTests, SerializeDeserializeState) {
    std::unordered_map<Address, Account, AddressHasher> accounts_;
    accounts_.insert({{0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                       0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A},
                      Account(1, 2, EVMC_ACCESS_COLD, 0)});
    State s(accounts_);
    bytes blob = s.serialize();

    State result = State::deserialize(blob);

    auto accounts = result.getAccounts();
    EXPECT_EQ(accounts.size(), 1);

    Address addr = {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A};
    EXPECT_TRUE(accounts.contains(addr));

    Account acc = accounts[addr];

    EXPECT_EQ(acc.getAccessStatus(), EVMC_ACCESS_COLD);
    EXPECT_EQ(acc.getBalance(), 2);
    EXPECT_EQ(acc.getNonce(), 1);
    EXPECT_EQ(acc.getStorageRoot(), 0);
}
