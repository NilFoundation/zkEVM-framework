#include <cstddef>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/block_header.hpp"

using namespace data_types;

TEST(DataTypesBlockHeaderTests, SerializeDeserializeBlockHeader) {
    bytes data = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
    BlockHeader hdr(0, 1, 2, 3, data, 5);
    bytes blob = hdr.serialize();

    BlockHeader result = BlockHeader::deserialize(blob);

    EXPECT_EQ(result.m_parentHash, 0);
    EXPECT_EQ(result.m_number, 1);
    EXPECT_EQ(result.m_gasLimit, 2);
    EXPECT_EQ(result.m_gasUsed, 3);
    EXPECT_THAT(result.m_extraData, testing::ElementsAreArray(data));
    EXPECT_EQ(result.m_timestamp, 5);
}
