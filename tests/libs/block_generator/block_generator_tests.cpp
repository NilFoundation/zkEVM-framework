#include <gtest/gtest.h>

#include <boost/json.hpp>
#include <boost/json/stream_parser.hpp>
#include <cstdint>
#include <fstream>
#include <initializer_list>

#include "zkevm_framework/block_generator/block_generator.hpp"
#include "zkevm_framework/data_types/block.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

namespace data_types::block_generator::test {

    std::expected<boost::json::value, std::string> parse_test_file(const char *path) {
        std::ifstream input_file(path);
        if (!input_file.is_open()) {
            return {};
        }
        return parse_json(input_file);
    }

    template<typename T>
    bool compare_bytes(const T &byte_container, std::initializer_list<uint8_t> expected) {
        if (byte_container.size() != expected.size()) {
            std::cout << "sizes" << std::endl;
            return false;
        }
        for (auto it = expected.begin(); it != expected.end(); ++it) {
            if (static_cast<typename T::value_type>(*it) != byte_container[it - expected.begin()]) {
                std::cout << std::hex << (int)*it << std::endl;
                return false;
            }
        }
        return true;
    }

    TEST(generator_test, smoke_test) {
        // Simply check if the format description was parsed without any errors
        auto jvalue = parse_test_file(INPUT_PATH "/inputs/scheme.json");
        ASSERT_TRUE(jvalue);
        auto res = generate_block(*jvalue);
        ASSERT_TRUE(res);
    }

    TEST(generator_test, block_components) {
        auto jvalue = parse_test_file(INPUT_PATH "/inputs/block.json");
        ASSERT_TRUE(jvalue);
        auto res = generate_block(*jvalue);
        ASSERT_TRUE(res);

        Block &block = res.value();
        EXPECT_EQ(block.m_accountBlocks.size(), 2);
        // Check account block
        auto &first_account_block = block.m_accountBlocks.front();
        EXPECT_EQ(first_account_block.m_transactions.size(), 2);
        EXPECT_EQ(first_account_block.m_transactions.back().m_id, 2);
        EXPECT_TRUE(compare_bytes(first_account_block.m_accountAddress,
                                  {0x5b, 0x0d, 0xd9, 0x3b, 0x8d, 0x86, 0x6d, 0x5d, 0x54, 0xe4,
                                   0xe4, 0xb5, 0x5e, 0xb5, 0xc6, 0x20, 0xcb, 0x9e, 0xaf, 0x53}));

        // Check transaction fields
        auto &transactiion_id_1 = first_account_block.m_transactions.front();
        EXPECT_EQ(transactiion_id_1.m_id, 1);
        EXPECT_EQ(transactiion_id_1.m_type, Transaction::ContractCreation);
        EXPECT_EQ(transactiion_id_1.m_nonce, 95);
        EXPECT_EQ(transactiion_id_1.m_value, 4);
        EXPECT_TRUE(compare_bytes(transactiion_id_1.m_receiveAddress,
                                  {0x9b, 0xfa, 0x97, 0xdd, 0x7c, 0x7b, 0xb1, 0xc4, 0xb4, 0xc8,
                                   0xbc, 0x5c, 0xf0, 0xec, 0x41, 0xee, 0xaf, 0x4d, 0x38, 0x8d}));
        EXPECT_EQ(transactiion_id_1.m_gasPrice, 8);
        EXPECT_EQ(transactiion_id_1.m_gas, 45000);
        EXPECT_TRUE(compare_bytes(
            transactiion_id_1.m_data,
            {0xb8, 0x86, 0xb9, 0xde, 0xc5, 0x0e, 0x17, 0x30, 0x7d, 0x0a, 0x55, 0x79,
             0xd8, 0xa3, 0x15, 0xb3, 0x3e, 0x98, 0xec, 0xe4, 0x84, 0xea, 0xc2, 0xc6}));
        EXPECT_TRUE(compare_bytes(transactiion_id_1.m_sender,
                                  {0xca, 0x4b, 0xd0, 0x1d, 0xaa, 0x33, 0x8b, 0xec, 0xcd, 0x57,
                                   0xf5, 0xe4, 0xa0, 0x75, 0x57, 0x79, 0x7f, 0x7b, 0x5e, 0x64}));

        // Check block header
        auto &block_header = block.m_currentBlock;
        EXPECT_EQ(block_header.m_parentHash, 128);
        EXPECT_EQ(block_header.m_number, 48);
        EXPECT_EQ(block_header.m_gasLimit, 500000);
        EXPECT_EQ(block_header.m_gasUsed, 133000);
        EXPECT_TRUE(compare_bytes(block_header.m_coinbase,
                                  {0x8e, 0x7e, 0x6a, 0x24, 0x63, 0x07, 0x9a, 0x0e, 0xbd, 0x8c,
                                   0xdd, 0xab, 0xba, 0x98, 0xa0, 0x86, 0xce, 0xaa, 0x5c, 0x27}));
        EXPECT_EQ(block_header.m_prevrandao, 36);
        EXPECT_EQ(block_header.m_chain_id, 3);
        EXPECT_EQ(block_header.m_basefee, 3);
        EXPECT_EQ(block_header.m_blob_basefee, 14);
        EXPECT_EQ(block_header.m_timestamp, 547675436597);

        // Check input messages
        EXPECT_EQ(block.m_inputMsgs.size(), 1);
        auto &input_msg = block.m_inputMsgs.front();
        EXPECT_TRUE(compare_bytes(input_msg.m_info.m_src,
                                  {0xca, 0x4b, 0xd0, 0x1d, 0xaa, 0x33, 0x8b, 0xec, 0xcd, 0x57,
                                   0xf5, 0xe4, 0xa0, 0x75, 0x57, 0x79, 0x7f, 0x7b, 0x5e, 0x64}));
        EXPECT_TRUE(compare_bytes(input_msg.m_info.m_dst,
                                  {0x9b, 0xfa, 0x97, 0xdd, 0x7c, 0x7b, 0xb1, 0xc4, 0xb4, 0xc8,
                                   0xbc, 0x5c, 0xf0, 0xec, 0x41, 0xee, 0xaf, 0x4d, 0x38, 0x8d}));
        EXPECT_EQ(input_msg.m_info.m_value, 4);
        EXPECT_EQ(input_msg.m_transaction.m_id, 1);

        // Check output messages
        EXPECT_EQ(block.m_inputMsgs.size(), 1);
        auto &output_msg = block.m_outputMsgs.front();
        EXPECT_TRUE(compare_bytes(output_msg.m_info.m_src,
                                  {0x46, 0x1f, 0xb6, 0xcb, 0x83, 0x43, 0x7a, 0x5a, 0x2a, 0xe3,
                                   0xe8, 0x36, 0xfd, 0x91, 0xa4, 0xf9, 0xa5, 0xf3, 0x9d, 0x4f}));
        EXPECT_TRUE(compare_bytes(output_msg.m_info.m_dst,
                                  {0xf4, 0xae, 0x4a, 0x37, 0xb3, 0x34, 0x61, 0xaa, 0x3b, 0xf0,
                                   0x3d, 0x16, 0x9c, 0x43, 0x7a, 0x4c, 0xa3, 0x10, 0xe9, 0x68}));
        EXPECT_EQ(output_msg.m_info.m_value, 1);
        EXPECT_EQ(output_msg.m_transaction.m_id, 2);
    }

    TEST(generator_test, transaction_errors) {
        auto correct_config = parse_test_file(INPUT_PATH "/inputs/block.json");
        ASSERT_TRUE(correct_config);

        auto first_transaction = [](boost::json::value &config) -> boost::json::object & {
            return config.as_object()["transactions"].as_array()[0].as_object();
        };

        auto unexpected_transaction_id_json = *correct_config;
        first_transaction(unexpected_transaction_id_json)["id"] = 5;
        auto result = generate_block(unexpected_transaction_id_json);
        EXPECT_FALSE(result);
        EXPECT_EQ(result.error(), "Unknown transaction id: 1");

        auto duplicated_transaction_id_json = *correct_config;
        first_transaction(duplicated_transaction_id_json)["id"] = 2;
        result = generate_block(duplicated_transaction_id_json);
        EXPECT_FALSE(result);
        EXPECT_EQ(result.error(), "Duplicated id of transaction: 2");

        auto bad_transaction_type_json = *correct_config;
        first_transaction(bad_transaction_type_json)["type"] = "Incorrect type";
        result = generate_block(bad_transaction_type_json);
        EXPECT_FALSE(result);
        EXPECT_EQ(
            result.error(),
            "Failed to match against any enum values. Location: <root>[transactions][0][type]");

        auto bad_transaction_sender_json = *correct_config;
        first_transaction(bad_transaction_sender_json)["sender"] =
            "0x9bfa97dd7c7bb1c4";  // Too short address
        result = generate_block(bad_transaction_sender_json);
        EXPECT_FALSE(result);
        EXPECT_EQ(result.error(),
                  "String should be no fewer than 42 characters in length. Location: "
                  "<root>[transactions][0][sender]");

        first_transaction(bad_transaction_sender_json)["sender"] =
            "non-hex string with same length as address";
        result = generate_block(bad_transaction_sender_json);
        EXPECT_FALSE(result);
        EXPECT_EQ(result.error(),
                  "Failed to match regex specified by 'pattern' constraint. Location: "
                  "<root>[transactions][0][sender]");

        first_transaction(bad_transaction_sender_json)["sender"] = 77;  // Unexpected type
        result = generate_block(bad_transaction_sender_json);
        EXPECT_FALSE(result);
        EXPECT_EQ(result.error(),
                  "Value type not permitted by 'type' constraint. Location: "
                  "<root>[transactions][0][sender]");

        auto extra_field_json = *correct_config;
        first_transaction(extra_field_json)["new_field"] = "some_value";
        result = generate_block(extra_field_json);
        EXPECT_FALSE(result);
        EXPECT_EQ(
            result.error(),
            "Object contains a property that could not be validated using 'properties' or "
            "'additionalProperties' constraints: 'new_field'. Location: <root>[transactions][0]");

        auto absent_field_json = *correct_config;
        first_transaction(absent_field_json)
            .erase(first_transaction(absent_field_json).find("sender"));
        result = generate_block(absent_field_json);
        EXPECT_FALSE(result);
        EXPECT_EQ(result.error(),
                  "Missing required property 'sender'. Location: <root>[transactions][0]");
    }
}  // namespace data_types::block_generator::test
