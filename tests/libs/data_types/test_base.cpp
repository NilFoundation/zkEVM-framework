#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "zkevm_framework/data_types/base.hpp"

using namespace data_types;

TEST(DataTypesAccountTests, ReadBytes) {
    std::stringstream s;
    s << "\xAA\xBB\xCC";
    auto blob = read_bytes(s);
    EXPECT_TRUE(blob);
    ASSERT_THAT(blob.value(),
                testing::ElementsAre(std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}));
}

TEST(DataTypesBaseTests, ReadBytesFails) {
    std::ifstream f("abracadabra");  // this file does not exist
    auto result = read_bytes(f);
    EXPECT_FALSE(result);
}

TEST(DataTypesBaseTests, WriteBytes) {
    bytes blob = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
    std::stringstream s;
    auto result = write_bytes(blob, s);
    EXPECT_TRUE(result);
    EXPECT_EQ(s.str(), "\xAA\xBB\xCC");
}

TEST(DataTypesBaseTests, WriteBytesFails) {
    bytes blob = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
    std::ofstream f("abracadabra", std::ios_base::in);  // wrong mode
    auto result = write_bytes(blob, f);
    EXPECT_FALSE(result);
}
