#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "sszpp/ssz++.hpp"

#include <array>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace testing;

// Fix-sized structure to serialize
struct FixedSize : ssz::ssz_container {
    std::array<std::uint32_t, 3> x;
    std::uint16_t y;

    // List of fields which will be serialized
    SSZ_CONT(x, y);
};

TEST(SSZTests, FixedSizeSerialization) {
    FixedSize example;
    example.x = {1, 2, 3};
    example.y = 4;

    std::vector<std::byte> blob = ssz::serialize(example);

    EXPECT_EQ(blob.size(), 14);

    // 4 bytes of `e.x[0]`
    EXPECT_EQ(blob[0], std::byte {0x01});
    EXPECT_EQ(blob[1], std::byte {0x00});
    EXPECT_EQ(blob[2], std::byte {0x00});
    EXPECT_EQ(blob[3], std::byte {0x00});

    // 4 bytes of `e.x[1]`
    EXPECT_EQ(blob[4], std::byte {0x02});
    EXPECT_EQ(blob[5], std::byte {0x00});
    EXPECT_EQ(blob[6], std::byte {0x00});
    EXPECT_EQ(blob[7], std::byte {0x00});

    // 4 bytes of `e.x[2]`
    EXPECT_EQ(blob[8], std::byte {0x03});
    EXPECT_EQ(blob[9], std::byte {0x00});
    EXPECT_EQ(blob[10], std::byte {0x00});
    EXPECT_EQ(blob[11], std::byte {0x00});

    // 2 bytes of `e.y`
    EXPECT_EQ(blob[12], std::byte {0x04});
    EXPECT_EQ(blob[13], std::byte {0x00});

    FixedSize result = ssz::deserialize<FixedSize>(blob);

    EXPECT_THAT(result.x, ElementsAre(1, 2, 3));

    EXPECT_EQ(result.y, 4);
}
