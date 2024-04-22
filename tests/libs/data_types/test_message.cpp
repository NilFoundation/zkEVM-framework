#include <sstream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/message.hpp"

using namespace data_types;

TEST(DataTypesMessageTests, SerializeDeserializeCommonMsgInfo) {
    Address src = {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                   0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A};
    Address dst = {0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
                   0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B};
    bytes data = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
    size_t value = 42;
    CommonMsgInfo info(src, dst, value);

    bytes blob = info.serialize();

    CommonMsgInfo result = CommonMsgInfo::deserialize(blob);

    EXPECT_EQ(result.m_src, src);
    EXPECT_EQ(result.m_dst, dst);
    EXPECT_EQ(result.m_value, value);
}

TEST(DataTypesMessageTests, StreamSerializeDeserializeCommonMsgInfo) {
    Address src = {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                   0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A};
    Address dst = {0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
                   0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B};
    bytes data = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
    size_t value = 42;
    CommonMsgInfo info(src, dst, value);

    std::stringstream ss;
    info.serialize(ss);

    std::optional<CommonMsgInfo> opt_result = CommonMsgInfo::deserialize(ss);
    EXPECT_TRUE(opt_result.has_value());
    CommonMsgInfo result = opt_result.value();

    EXPECT_EQ(result.m_src, src);
    EXPECT_EQ(result.m_dst, dst);
    EXPECT_EQ(result.m_value, value);
}

TEST(DataTypesMessageTests, SerializeDeserializeInMsg) {
    Address src = {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                   0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A};
    Address dst = {0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
                   0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B};
    bytes data = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
    size_t value = 42;
    Transaction transaction(0, Transaction::Type::ContractCreation, 1, 2, dst, 3, 4, data, src);
    CommonMsgInfo info(src, dst, value);
    InMsg msg(info, transaction);

    bytes blob = msg.serialize();

    InMsg result = InMsg::deserialize(blob);

    EXPECT_EQ(result.m_info.m_src, src);
    EXPECT_EQ(result.m_info.m_dst, dst);
    EXPECT_EQ(result.m_info.m_value, value);
    Transaction msg_t = result.m_transaction;
    EXPECT_EQ(msg_t.m_id, 0);
    EXPECT_EQ(msg_t.m_type, Transaction::Type::ContractCreation);
    EXPECT_EQ(msg_t.m_nonce, 1);
    EXPECT_EQ(msg_t.m_value, 2);
    EXPECT_EQ(msg_t.m_gasPrice, 3);
    EXPECT_EQ(msg_t.m_gas, 4);
    EXPECT_THAT(msg_t.m_receiveAddress, testing::ElementsAreArray(dst));
    EXPECT_THAT(msg_t.m_sender, testing::ElementsAreArray(src));
    EXPECT_THAT(msg_t.m_data, testing::ElementsAreArray(data));
}

TEST(DataTypesMessageTests, StreamSerializeDeserializeInMsg) {
    Address src = {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                   0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A};
    Address dst = {0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
                   0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B};
    bytes data = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
    size_t value = 42;
    Transaction transaction(0, Transaction::Type::ContractCreation, 1, 2, dst, 3, 4, data, src);
    CommonMsgInfo info(src, dst, value);
    InMsg msg(info, transaction);

    std::stringstream ss;
    msg.serialize(ss);

    std::optional<InMsg> opt_result = InMsg::deserialize(ss);
    EXPECT_TRUE(opt_result.has_value());
    InMsg result = opt_result.value();

    EXPECT_EQ(result.m_info.m_src, src);
    EXPECT_EQ(result.m_info.m_dst, dst);
    EXPECT_EQ(result.m_info.m_value, value);
    Transaction msg_t = result.m_transaction;
    EXPECT_EQ(msg_t.m_id, 0);
    EXPECT_EQ(msg_t.m_type, Transaction::Type::ContractCreation);
    EXPECT_EQ(msg_t.m_nonce, 1);
    EXPECT_EQ(msg_t.m_value, 2);
    EXPECT_EQ(msg_t.m_gasPrice, 3);
    EXPECT_EQ(msg_t.m_gas, 4);
    EXPECT_THAT(msg_t.m_receiveAddress, testing::ElementsAreArray(dst));
    EXPECT_THAT(msg_t.m_sender, testing::ElementsAreArray(src));
    EXPECT_THAT(msg_t.m_data, testing::ElementsAreArray(data));
}

TEST(DataTypesMessageTests, SerializeDeserializeOutMsg) {
    Address src = {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                   0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A};
    Address dst = {0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
                   0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B};
    bytes data = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
    size_t value = 42;
    Transaction transaction(0, Transaction::Type::ContractCreation, 1, 2, dst, 3, 4, data, src);
    CommonMsgInfo info(src, dst, value);
    OutMsg msg(info, transaction);

    bytes blob = msg.serialize();

    OutMsg result = OutMsg::deserialize(blob);

    EXPECT_EQ(result.m_info.m_src, src);
    EXPECT_EQ(result.m_info.m_dst, dst);
    EXPECT_EQ(result.m_info.m_value, value);
    Transaction msg_t = result.m_transaction;
    EXPECT_EQ(msg_t.m_id, 0);
    EXPECT_EQ(msg_t.m_type, Transaction::Type::ContractCreation);
    EXPECT_EQ(msg_t.m_nonce, 1);
    EXPECT_EQ(msg_t.m_value, 2);
    EXPECT_EQ(msg_t.m_gasPrice, 3);
    EXPECT_EQ(msg_t.m_gas, 4);
    EXPECT_THAT(msg_t.m_receiveAddress, testing::ElementsAreArray(dst));
    EXPECT_THAT(msg_t.m_sender, testing::ElementsAreArray(src));
    EXPECT_THAT(msg_t.m_data, testing::ElementsAreArray(data));
}

TEST(DataTypesMessageTests, StreamSerializeDeserializeOutMsg) {
    Address src = {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
                   0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A};
    Address dst = {0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
                   0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B};
    bytes data = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
    size_t value = 42;
    Transaction transaction(0, Transaction::Type::ContractCreation, 1, 2, dst, 3, 4, data, src);
    CommonMsgInfo info(src, dst, value);
    OutMsg msg(info, transaction);

    std::stringstream ss;
    msg.serialize(ss);

    std::optional<OutMsg> opt_result = OutMsg::deserialize(ss);
    EXPECT_TRUE(opt_result.has_value());
    OutMsg result = opt_result.value();

    EXPECT_EQ(result.m_info.m_src, src);
    EXPECT_EQ(result.m_info.m_dst, dst);
    EXPECT_EQ(result.m_info.m_value, value);
    Transaction msg_t = result.m_transaction;
    EXPECT_EQ(msg_t.m_id, 0);
    EXPECT_EQ(msg_t.m_type, Transaction::Type::ContractCreation);
    EXPECT_EQ(msg_t.m_nonce, 1);
    EXPECT_EQ(msg_t.m_value, 2);
    EXPECT_EQ(msg_t.m_gasPrice, 3);
    EXPECT_EQ(msg_t.m_gas, 4);
    EXPECT_THAT(msg_t.m_receiveAddress, testing::ElementsAreArray(dst));
    EXPECT_THAT(msg_t.m_sender, testing::ElementsAreArray(src));
    EXPECT_THAT(msg_t.m_data, testing::ElementsAreArray(data));
}
