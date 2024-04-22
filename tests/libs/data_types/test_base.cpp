#include <cstddef>
#include <sstream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "zkevm_framework/data_types/base.hpp"

using namespace data_types;

TEST(DataTypesBaseTests, ReadBytes) {
    bytes blob = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
    std::stringstream s;
    int count = write_bytes(blob, s);
    EXPECT_TRUE(!s.fail());
    EXPECT_EQ(count, 3);
    EXPECT_EQ(s.str(), "\xAA\xBB\xCC");
}

TEST(DataTypesAccountTests, WriteBytes) {
    std::stringstream s;
    s << "\xAA\xBB\xCC";
    std::optional<bytes> blob = read_bytes(s);
    EXPECT_TRUE(!s.fail());
    EXPECT_TRUE(blob.has_value());
    ASSERT_THAT(blob.value(),
                testing::ElementsAre(std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}));
}
