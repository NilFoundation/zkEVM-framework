#include "zkevm_framework/block_generator/block_generator.hpp"

#include <boost/json.hpp>
#include <boost/json/array.hpp>
#include <boost/json/serialize.hpp>
#include <boost/json/stream_parser.hpp>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "block_schema.def"
#include "zkevm_framework/data_types/account_block.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/block.hpp"
#include "zkevm_framework/data_types/block_header.hpp"
#include "zkevm_framework/data_types/message.hpp"
#include "zkevm_framework/data_types/transaction.hpp"
#include "zkevm_framework/json_helpers/json_helpers.hpp"

namespace {
    std::unordered_map<size_t, data_types::Transaction> transaction_map;

    // Iterate over keys in the entity and call handler for each of them
    template<typename Callback, typename Result>
    void iterate_object(const boost::json::object &entity, Result &res, Callback key_handler) {
        for (const auto &elem : entity) {
            key_handler(elem.key_c_str(), elem.value(), res);
        }
    }

    // Transform hex string into Address or bytes
    template<typename OutputContainer>
    void get_bytes(const boost::json::value hex_value, OutputContainer &result) {
        std::vector<uint8_t> bytes = json_helpers::get_bytes(hex_value);
        if constexpr (std::is_same_v<OutputContainer, data_types::Address>) {
            std::copy(bytes.begin(), bytes.end(), result.begin());
        } else {
            // Store the data of a transaction, could be any size
            result.clear();
            for (uint8_t b : bytes) {
                result.push_back(std::byte(b));
            }
        }
    }

    using json_helpers::get_number;

    void handle_transaction(const std::string &key, const boost::json::value &json_value,
                            data_types::Transaction &transaction) {
        if (key == "id") {
            transaction.m_id = get_number(json_value);
        } else if (key == "type") {
            auto type_string = json_value.as_string();
            if (type_string == "ContractCreation") {
                transaction.m_type = data_types::Transaction::ContractCreation;
            } else if (type_string == "NullTransaction") {
                transaction.m_type = data_types::Transaction::NullTransaction;
            } else {
                transaction.m_type = data_types::Transaction::MessageCall;
            }
        } else if (key == "nonce") {
            transaction.m_nonce = get_number(json_value);
        } else if (key == "value") {
            transaction.m_value = get_number(json_value);
        } else if (key == "receive_address") {
            get_bytes(json_value, transaction.m_receiveAddress);
        } else if (key == "gas_price") {
            transaction.m_gasPrice = get_number(json_value);
        } else if (key == "gas") {
            transaction.m_gas = get_number(json_value);
        } else if (key == "data") {
            get_bytes(json_value, transaction.m_data);
        } else if (key == "sender") {
            get_bytes(json_value, transaction.m_sender);
        }
    }

    void handle_block_header(const std::string &key, const boost::json::value &json_value,
                             data_types::BlockHeader &block_header) {
        if (key == "parent_hash") {
            block_header.m_parentHash = get_number(json_value);
        } else if (key == "number") {
            block_header.m_number = get_number(json_value);
        } else if (key == "gas_limit") {
            block_header.m_gasLimit = get_number(json_value);
        } else if (key == "gas_used") {
            block_header.m_gasUsed = get_number(json_value);
        } else if (key == "coinbase") {
            get_bytes(json_value, block_header.m_coinbase);
        } else if (key == "prevrandao") {
            block_header.m_prevrandao = get_number(json_value);
        } else if (key == "chain_id") {
            block_header.m_chain_id = get_number(json_value);
        } else if (key == "basefee") {
            block_header.m_basefee = get_number(json_value);
        } else if (key == "blob_basefee") {
            block_header.m_blob_basefee = get_number(json_value);
        } else if (key == "timestamp") {
            block_header.m_timestamp = get_number(json_value);
        }
    }

    void handle_account_block(const std::string &key, const boost::json::value &json_value,
                              data_types::AccountBlock &account_block) {
        if (key == "account_address") {
            get_bytes(json_value, account_block.m_accountAddress);
        } else if (key == "transactions") {
            for (const auto &array_elem : json_value.as_array()) {
                size_t idx = get_number(array_elem);
                auto it = transaction_map.find(idx);
                account_block.m_transactions.push_back(it->second);
            }
        }
    }

    template<typename MessageType>
    void handle_message(const std::string &key, const boost::json::value &json_value,
                        MessageType &msg) {
        if (key == "src") {
            get_bytes(json_value, msg.m_info.m_src);
        } else if (key == "dst") {
            get_bytes(json_value, msg.m_info.m_dst);
        } else if (key == "value") {
            msg.m_info.m_value = get_number(json_value);
        } else if (key == "transaction") {
            size_t idx = get_number(json_value);
            auto it = transaction_map.find(idx);
            msg.m_transaction = it->second;
        }
    }

    void handle_high_level_block(const std::string &key, const boost::json::value &json_value,
                                 data_types::Block &block) {
        if (key == "previous_header") {
            iterate_object(json_value.as_object(), block.m_previousBlock, handle_block_header);
        } else if (key == "current_header") {
            iterate_object(json_value.as_object(), block.m_currentBlock, handle_block_header);
        } else if (key == "transactions") {
            // Already processed
        } else if (key == "account_blocks") {
            for (const auto &elem : json_value.as_array()) {
                auto account_block_json = elem.as_object();
                data_types::AccountBlock ab;
                block.m_accountBlocks.push_back(ab);
                iterate_object(account_block_json, block.m_accountBlocks.back(),
                               handle_account_block);
            }
        } else if (key == "input_messages") {
            for (const auto &elem : json_value.as_array()) {
                auto input_message_json = elem.as_object();
                data_types::InMsg msg;
                block.m_inputMsgs.push_back(msg);
                iterate_object(input_message_json, block.m_inputMsgs.back(),
                               handle_message<data_types::InMsg>);
            }
        } else if (key == "output_messages") {
            for (const auto &elem : json_value.as_array()) {
                auto output_message_json = elem.as_object();
                data_types::OutMsg msg;
                block.m_outputMsgs.push_back(msg);
                iterate_object(output_message_json, block.m_outputMsgs.back(),
                               handle_message<data_types::OutMsg>);
            }
        }
    }

    std::optional<std::string> check_json_config(const boost::json::value &json_value) {
        // Parse schema and check if the config is valid
        std::stringstream schema_stream;
        schema_stream << block_schema;
        std::expected<boost::json::value, std::string> schema =
            json_helpers::parse_json(schema_stream);
        if (!schema) {
            return "Parsing of json schema is failed: " + schema.error();
        }

        auto validation_error = json_helpers::validate_json(schema.value(), json_value);
        if (validation_error) {
            return validation_error;
        }

        // Collect transaction ids
        std::unordered_set<size_t> ids;
        const boost::json::object &block_json = json_value.as_object();
        for (const auto &transaction_json : block_json.at("transactions").as_array()) {
            size_t id = get_number(transaction_json.as_object().at("id"));
            if (ids.contains(id)) {
                return "Duplicated id of transaction: " + std::to_string(id);
            }
            ids.insert(id);
        }

        // Check if account blocks contain correct transaction ids
        for (const auto &account_block_json : block_json.at("account_blocks").as_array()) {
            const auto &transaction_ids_json = account_block_json.as_object().at("transactions");
            for (const auto &id_json : transaction_ids_json.as_array()) {
                size_t id = get_number(id_json);
                if (!ids.contains(id)) {
                    return "Unknown transaction id: " + std::to_string(id);
                }
            }
        }

        // Same for messages
        auto validate_message_transactions =
            [&ids](const boost::json::array &msg_array) -> std::optional<std::string> {
            for (const auto &msg : msg_array) {
                size_t id = get_number(msg.as_object().at("transaction"));
                if (!ids.contains(id)) {
                    return "Unknown transaction id: " + std::to_string(id);
                }
            }
            return {};
        };
        std::optional<std::string> msg_err =
            validate_message_transactions(block_json.at("input_messages").as_array());
        if (msg_err) {
            return msg_err;
        }
        return validate_message_transactions(block_json.at("output_messages").as_array());
    }

    void fill_block(const boost::json::value &json_value, data_types::Block &res) {
        // Handle transactions first to fill the map
        transaction_map.clear();
        auto json_block = json_value.as_object();
        for (const auto &elem : json_block["transactions"].as_array()) {
            auto json_transaction = elem.as_object();
            size_t transaction_id = get_number(json_transaction["id"]);
            transaction_map[transaction_id] = data_types::Transaction();
            iterate_object(json_transaction, transaction_map[transaction_id], handle_transaction);
        }

        // Handle other fields
        iterate_object(json_value.as_object(), res, handle_high_level_block);
    }
}  // namespace

namespace data_types::block_generator {
    std::expected<data_types::Block, std::string> generate_block(
        const boost::json::value &json_value) {
        auto validation_err = check_json_config(json_value);
        if (validation_err) {
            return std::unexpected<std::string>(*validation_err);
        }

        data_types::Block res;
        fill_block(json_value, res);
        return res;
    }
}  // namespace data_types::block_generator
