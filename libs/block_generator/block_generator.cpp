#include "block_generator.hpp"

#include <boost/algorithm/hex.hpp>
#include <boost/json/serialize.hpp>
#include <boost/json/stream_parser.hpp>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "zkevm_framework/data_types/account_block.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/block.hpp"
#include "zkevm_framework/data_types/block_header.hpp"
#include "zkevm_framework/data_types/message.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

namespace {

    // Helpers for better diagnostic of errors related to wrong keys
    template<typename T>
    constexpr const char *result_type_name(const T &);

    // Transaction
    std::unordered_set<const char *> transaction_keys = {
        "id", "type", "nonce", "value", "receive_address", "gas_price", "gas", "data", "sender",
    };

    template<>
    constexpr const char *result_type_name(const data_types::Transaction &) {
        return "data_types::Transaction";
    }

    // Block
    std::unordered_set<const char *> block_keys = {
        "previous_header", "current_header", "transactions",
        "account_blocks",  "input_messages", "output_messages",
    };

    template<>
    constexpr const char *result_type_name(const data_types::Block &) {
        return "data_types::Block";
    }

    // AccountBlock
    std::unordered_set<const char *> account_block_keys = {
        "account_address",
        "transactions",
    };

    template<>
    constexpr const char *result_type_name(const data_types::AccountBlock &) {
        return "data_types::AccountBlock";
    }

    std::unordered_set<const char *> block_header_keys = {
        "parent_hash", "number",   "gas_limit", "gas_used",     "coinbase",
        "prevrandao",  "chain_id", "basefee",   "blob_basefee", "timestamp",
    };

    template<>
    constexpr const char *result_type_name(const data_types::BlockHeader &) {
        return "data_types::BlockHeader";
    }

    // Messages
    std::unordered_set<const char *> message_keys = {
        "src",
        "dst",
        "value",
        "transaction",
    };

    template<>
    constexpr const char *result_type_name(const data_types::InMsg &) {
        return "data_types::InMsg";
    }

    template<>
    constexpr const char *result_type_name(const data_types::OutMsg &) {
        return "data_types::OutMsg";
    }

    std::unordered_map<size_t, data_types::Transaction> transaction_map;
    std::ostringstream error_msg;

    // Iterate over keys in the entity and call handler for each of them
    template<typename Callback, typename Result>
    bool iterate_object(const boost::json::object &entity,
                        std::unordered_set<const char *> expected_keys, Result &res,
                        Callback key_handler) {
        if (entity.size() != expected_keys.size()) {
            // Skipping of some fields is not allowed in json file
            error_msg << "Incorrect set of keys for " << result_type_name(res);
            return false;
        }
        for (const auto &elem : entity) {
            const auto &key = elem.key_c_str();
            auto expected_it = std::find_if(
                expected_keys.begin(), expected_keys.end(),
                [&elem](const char *expected_key) { return elem.key() == expected_key; });
            if (expected_it == expected_keys.end()) {
                error_msg << "Unexpected key '" << elem.key() << "' for " << result_type_name(res);
                return false;
            }
            if (!key_handler(elem.key_c_str(), elem.value(), res)) {
                return false;
            }
        }
        return true;
    }

    bool get_number(const std::string &key, const boost::json::value &json_value, size_t &result) {
        if (json_value.is_uint64()) {
            result = json_value.as_uint64();
        } else if (json_value.is_int64()) {
            result = json_value.as_int64();
        } else {
            error_msg << "Value '" << boost::json::serialize(json_value) << "' of '" << key
                      << "' key is not a number";
            return false;
        }
        return true;
    }

    // Transform hex string into Address or bytes
    template<typename OutputContainer>
    static bool get_bytes(const std::string &key, const boost::json::value hex_value,
                          OutputContainer &result) {
        if (!hex_value.is_string()) {
            error_msg << "Value '" << boost::json::serialize(hex_value) << "' of '" << key
                      << "' is not a string";
            return false;
        }
        std::string hex_string = hex_value.as_string().c_str();
        std::vector<uint8_t> bytes;
        // Skip 0x prefix if exists
        if (hex_string[0] == '0' && hex_string[1] == 'x') {
            hex_string.erase(0, 2);
        }
        try {
            boost::algorithm::unhex(hex_string, std::back_inserter(bytes));
        } catch (const boost::algorithm::non_hex_input &ex) {
            error_msg << "Value " << boost::json::serialize(hex_value) << " of '" << key
                      << "' is not a hexadecimal string";
            return false;
        }
        if constexpr (std::is_same_v<OutputContainer, data_types::Address>) {
            if (bytes.size() != result.size()) {
                error_msg << "Wrong size of " << boost::json::serialize(hex_value) << " value of '"
                          << key << "', expected " << result.size() << " bytes";
                return false;
            }
            std::copy(bytes.begin(), bytes.end(), result.begin());
        } else {
            // Store the data of a transaction, could be any size
            result.clear();
            for (uint8_t b : bytes) {
                result.push_back(std::byte(b));
            }
        }
        return true;
    }

    bool handle_transaction(const std::string &key, const boost::json::value &json_value,
                            data_types::Transaction &transaction) {
        if (key == "id") {
            if (!get_number(key, json_value, transaction.m_id)) return false;
        } else if (key == "type") {
            if (!json_value.is_string()) {
                error_msg << "Value '" << boost::json::serialize(json_value)
                          << "' of key 'type' is not a string";
                return false;
            }
            auto type_string = json_value.as_string();
            if (type_string == "ContractCreation") {
                transaction.m_type = data_types::Transaction::ContractCreation;
            } else if (type_string == "NullTransaction") {
                transaction.m_type = data_types::Transaction::NullTransaction;
            } else if (type_string == "MessageCall") {
                transaction.m_type = data_types::Transaction::MessageCall;
            } else {
                error_msg << "Unexpected type of transaction: " << type_string;
                return false;
            }
        } else if (key == "nonce") {
            if (!get_number(key, json_value, transaction.m_nonce)) return false;
        } else if (key == "value") {
            if (!get_number(key, json_value, transaction.m_value)) return false;
        } else if (key == "receive_address") {
            if (!get_bytes(key, json_value, transaction.m_receiveAddress)) return false;
        } else if (key == "gas_price") {
            if (!get_number(key, json_value, transaction.m_gasPrice)) return false;
        } else if (key == "gas") {
            if (!get_number(key, json_value, transaction.m_gas)) return false;
        } else if (key == "data") {
            if (!get_bytes(key, json_value, transaction.m_data)) return false;
        } else if (key == "sender") {
            if (!get_bytes(key, json_value, transaction.m_sender)) return false;
        } else {
            return false;
        }
        return true;
    }

    bool handle_block_header(const std::string &key, const boost::json::value &json_value,
                             data_types::BlockHeader &block_header) {
        if (key == "parent_hash") {
            if (!get_number(key, json_value, block_header.m_parentHash)) return false;
        } else if (key == "number") {
            if (!get_number(key, json_value, block_header.m_number)) return false;
        } else if (key == "gas_limit") {
            if (!get_number(key, json_value, block_header.m_gasLimit)) return false;
        } else if (key == "gas_used") {
            if (!get_number(key, json_value, block_header.m_gasUsed)) return false;
        } else if (key == "coinbase") {
            if (!get_bytes(key, json_value, block_header.m_coinbase)) return false;
        } else if (key == "prevrandao") {
            if (!get_number(key, json_value, block_header.m_prevrandao)) return false;
        } else if (key == "chain_id") {
            if (!get_number(key, json_value, block_header.m_chain_id)) return false;
        } else if (key == "basefee") {
            if (!get_number(key, json_value, block_header.m_basefee)) return false;
        } else if (key == "blob_basefee") {
            if (!get_number(key, json_value, block_header.m_blob_basefee)) return false;
        } else if (key == "timestamp") {
            if (!get_number(key, json_value, block_header.m_timestamp)) return false;
        } else {
            return false;
        }
        return true;
    }

    bool handle_account_block(const std::string &key, const boost::json::value &json_value,
                              data_types::AccountBlock &account_block) {
        if (key == "account_address") {
            if (!get_bytes(key, json_value, account_block.m_accountAddress)) return false;
        } else if (key == "transactions") {
            if (!json_value.is_array()) {
                error_msg << "'transactions' of an account block must be an array, got '"
                          << boost::json::serialize(json_value) << "'";
            }
            for (const auto &array_elem : json_value.as_array()) {
                size_t idx;
                if (!get_number("transactions", array_elem, idx)) return false;
                auto it = transaction_map.find(idx);
                if (it == transaction_map.end()) {
                    error_msg << "Unknown transaction id: " << idx;
                    return false;
                }
                account_block.m_transactions.push_back(it->second);
            }
        }
        return true;
    }

    template<typename MessageType>
    bool handle_message(const std::string &key, const boost::json::value &json_value,
                        MessageType &msg) {
        if (key == "src") {
            if (!get_bytes(key, json_value, msg.m_info.m_src)) return false;
        } else if (key == "dst") {
            if (!get_bytes(key, json_value, msg.m_info.m_dst)) return false;
        } else if (key == "value") {
            if (!get_number(key, json_value, msg.m_info.m_value)) return false;
        } else if (key == "transaction") {
            size_t idx;
            if (!get_number(key, json_value, idx)) return false;
            auto it = transaction_map.find(idx);
            if (it == transaction_map.end()) {
                error_msg << "Unknown transaction id: " << idx;
                return false;
            }
            msg.m_transaction = it->second;
        } else {
            return false;
        }
        return true;
    }

    data_types::BlockHeader get_empty_block_header() {
        return data_types::BlockHeader(0, 0, 0, 0, data_types::Address(), 0, 0, 0, 0,
                                       data_types::bytes());
    }

    data_types::Transaction get_empty_transaction() {
        return data_types::Transaction(0, data_types::Transaction::ContractCreation, 0, 0,
                                       data_types::Address(), 0, 0, data_types::bytes(),
                                       data_types::Address());
    }

    data_types::CommonMsgInfo get_empty_message_info() {
        return data_types::CommonMsgInfo(data_types::Address(), data_types::Address(), 0);
    }

    bool handle_high_level_block(const std::string &key, const boost::json::value &json_value,
                                 data_types::Block &block) {
        if (key == "previous_header") {
            if (!json_value.is_object()) {
                error_msg << "Value '" << boost::json::serialize(json_value)
                          << "' of key 'previous_header' is not an object";
                return false;
            }
            if (!iterate_object(json_value.as_object(), block_header_keys, block.m_previousBlock,
                                handle_block_header))
                return false;
        } else if (key == "current_header") {
            if (!json_value.is_object()) {
                error_msg << "Value '" << boost::json::serialize(json_value)
                          << "' of key 'current_header' is not an object";
                return false;
            }
            if (!iterate_object(json_value.as_object(), block_header_keys, block.m_currentBlock,
                                handle_block_header))
                return false;
        } else if (key == "transactions") {
            // Already processed
            return true;
        } else if (key == "account_blocks") {
            if (!json_value.is_array()) {
                error_msg << "Value '" << boost::json::serialize(json_value)
                          << "' of key 'account_blocks' is not an array";
                return false;
            }
            for (const auto &elem : json_value.as_array()) {
                if (!elem.is_object()) {
                    error_msg << "Element '" << boost::json::serialize(elem)
                              << "' of array 'account_blocks' is not an object";
                    return false;
                }
                auto account_block_json = elem.as_object();
                data_types::AccountBlock ab(data_types::Address(), {});
                block.m_accountBlocks.push_back(ab);
                if (!iterate_object(account_block_json, account_block_keys,
                                    block.m_accountBlocks.back(), handle_account_block)) {
                    return false;
                }
            }
        } else if (key == "input_messages") {
            if (!json_value.is_array()) {
                error_msg << "Value '" << boost::json::serialize(json_value)
                          << "' of key 'input_messages' is not an array";
                return false;
            }
            for (const auto &elem : json_value.as_array()) {
                if (!elem.is_object()) {
                    error_msg << "Element '" << boost::json::serialize(elem)
                              << "' of array 'input_messages' is not an object";
                    return false;
                }
                auto input_message_json = elem.as_object();
                data_types::InMsg msg(get_empty_message_info(), get_empty_transaction());
                block.m_inputMsgs.push_back(msg);
                if (!iterate_object(input_message_json, message_keys, block.m_inputMsgs.back(),
                                    handle_message<data_types::InMsg>)) {
                    return false;
                }
            }
        } else if (key == "output_messages") {
            if (!json_value.is_array()) {
                error_msg << "Value '" << boost::json::serialize(json_value)
                          << "' of key 'output_messages' is not an array";
                return false;
            }
            for (const auto &elem : json_value.as_array()) {
                if (!elem.is_object()) {
                    error_msg << "Element '" << boost::json::serialize(elem)
                              << "' of array 'output_messages' is not an object";
                    return false;
                }
                auto output_message_json = elem.as_object();
                data_types::OutMsg msg(get_empty_message_info(), get_empty_transaction());
                block.m_outputMsgs.push_back(msg);
                if (!iterate_object(output_message_json, message_keys, block.m_outputMsgs.back(),
                                    handle_message<data_types::OutMsg>)) {
                    return false;
                }
            }
        } else {
            return false;
        }
        return true;
    }
}  // namespace

namespace data_types::block_generator {
    std::expected<data_types::Block, std::string> generate_block(
        const boost::json::value &json_value) {
        if (!json_value.is_object()) {
            return std::unexpected<std::string>("Config file must contain JSON object");
        }

        // Handle transactions first to fill the map
        transaction_map.clear();
        auto json_block = json_value.as_object();
        if (!json_block.contains("transactions")) {
            return std::unexpected<std::string>("Incorrect set of keys for data_types::Block");
        }
        if (!json_block["transactions"].is_array()) {
            auto error_msg = "Value '" + boost::json::serialize(json_block["transactions"]) +
                             "' of key 'transactions' must be an array";
            return std::unexpected<std::string>(error_msg);
        }
        error_msg.str("");
        for (const auto &elem : json_block["transactions"].as_array()) {
            if (!elem.is_object()) {
                auto error = "Element '" + boost::json::serialize(elem) +
                             "' of an array 'transactions' must be an object";
                return std::unexpected<std::string>(error);
            }
            auto json_transaction = elem.as_object();
            if (!json_transaction.contains("id")) {
                auto error = "Element '" + boost::json::serialize(elem) +
                             "' of an array 'transactions' must contain an 'id' key";
                return std::unexpected<std::string>(error);
            }
            size_t transaction_id;
            if (!get_number("id", json_transaction["id"], transaction_id)) {
                auto error = "Value '" + boost::json::serialize(json_transaction["id"]) +
                             "' of a key 'id' is not a number";
                return std::unexpected<std::string>(error);
            }
            if (transaction_map.find(transaction_id) != transaction_map.end()) {
                // Duplicated id of transaction
                auto error = "Duplicated id of transaction: " + std::to_string(transaction_id);
                return std::unexpected<std::string>(error);
            }
            transaction_map[transaction_id] = get_empty_transaction();
            if (!iterate_object(json_transaction, transaction_keys, transaction_map[transaction_id],
                                handle_transaction)) {
                return std::unexpected<std::string>(error_msg.str());
            }
        }

        data_types::Block res({}, {}, {}, get_empty_block_header(), get_empty_block_header());
        if (!iterate_object(json_value.as_object(), block_keys, res, handle_high_level_block)) {
            return std::unexpected<std::string>(error_msg.str());
        }
        return res;
    }

    std::optional<boost::json::value> parse_json(std::istream &stream) {
        boost::json::stream_parser p;
        boost::json::error_code ec;
        while (!stream.eof()) {
            char input_string[256];
            stream.read(input_string, sizeof(input_string) - 1);
            input_string[stream.gcount()] = '\0';
            p.write(input_string, ec);
            if (ec) {
                return {};
            }
        }
        p.finish(ec);
        if (ec) {
            return {};
        }
        return p.release();
    }
}  // namespace data_types::block_generator
