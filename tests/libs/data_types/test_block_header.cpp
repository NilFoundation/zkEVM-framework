#include <cstddef>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/block_header.hpp"

using namespace data_types;

TEST(DataTypesBlockHeaderTests, SerializeDeserializeBlockHeader) {
    bytes data = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
    Address author = {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                      0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A};
    BlockHeader hdr(0, 0, 0, 0, 1, 2, 3, data, author, 5);
    bytes blob = hdr.serialize();

    BlockHeader result = BlockHeader::deserialize(blob);

    EXPECT_EQ(result.parentHash(), 0);
    EXPECT_EQ(result.stateRoot(), 0);
    EXPECT_EQ(result.transactionsRoot(), 0);
    EXPECT_EQ(result.receiptsRoot(), 0);
    EXPECT_EQ(result.number(), 1);
    EXPECT_EQ(result.gasLimit(), 2);
    EXPECT_EQ(result.gasUsed(), 3);
    EXPECT_THAT(result.extraData(),
                testing::ElementsAre(std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}));
    EXPECT_THAT(result.author(),
                testing::ElementsAre(0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                                     0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A));
    EXPECT_EQ(result.timestamp(), 5);
}
