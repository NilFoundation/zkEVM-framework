#include "zkevm_framework/assigner_runner/state_parser.hpp"

#include <boost/algorithm/hex.hpp>
#include <boost/endian.hpp>
#include <boost/endian/conversion.hpp>
#include <cstring>
#include <evmc/evmc.hpp>
#include <fstream>
#include <iostream>

#include "state_schema.def"
#include "vm_host.h"
#include "zkevm_framework/json_helpers/json_helpers.hpp"

// Transform hex string into Address or bytes
template<typename OutputContainer>
static void get_bytes(const boost::json::value hex_value, OutputContainer &result) {
    std::vector<uint8_t> bytes = json_helpers::get_bytes(hex_value);
    if constexpr (std::is_same_v<OutputContainer, evmc::address>) {
        std::copy(bytes.begin(), bytes.end(), result.bytes);
    } else if constexpr (std::is_same_v<OutputContainer, evmc::bytes32>) {
        size_t zero_padding = 32 - bytes.size();
        std::memset(result.bytes, 0, zero_padding);
        std::copy(bytes.begin(), bytes.end(), result.bytes + zero_padding);
    } else {
        // Store the data of a transaction, could be any size
        result.clear();
        for (uint8_t b : bytes) {
            result.push_back(b);
        }
    }
}

static void handle_storage(const boost::json::array &storage_json, evmc::account &account) {
    auto &storage = account.storage;
    for (const auto &item : storage_json) {
        const boost::json::object &storage_elem = item.as_object();
        evmc::bytes32 key, value;
        for (const auto &elem : storage_elem) {
            if (elem.key() == "key") {
                get_bytes(elem.value(), key);
            } else if (elem.key() == "value") {
                get_bytes(elem.value(), value);
            }
        }
        storage.emplace(key, value);
    }
}

static void handle_account(const boost::json::object &account_obj, evmc::account &account) {
    for (const auto &elem : account_obj) {
        if (elem.key() == "balance") {
            size_t balance_value = json_helpers::get_number(elem.value());
            // Convert to big endian number as it required in evmc
            boost::endian::native_to_big_inplace(balance_value);
            // Set last bytes with balance value and others with zeros
            constexpr size_t zero_padding = sizeof(account.balance.bytes) - sizeof(balance_value);
            std::memcpy(account.balance.bytes + zero_padding, &balance_value,
                        sizeof(balance_value));
            std::memset(account.balance.bytes, 0, zero_padding);
        } else if (elem.key() == "address") {
            // Already processed
        } else if (elem.key() == "code") {
            get_bytes(elem.value(), account.code);
        } else if (elem.key() == "storage") {
            handle_storage(elem.value().as_array(), account);
        }
    }
}

std::optional<std::string> init_account_storage(evmc::accounts &account_storage,
                                                std::istream &config) {
    auto config_json = json_helpers::parse_json(config);
    if (!config_json) {
        return "Error while parsing state config file: " + config_json.error();
    }
    std::stringstream schema_stream;
    schema_stream << state_schema;
    auto schema_json = json_helpers::parse_json(schema_stream);
    if (!schema_json) {
        std::cerr << "Wrong schema" << std::endl;
        return "Error while parsing state schema: " + schema_json.error();
    }
    auto validation_err = json_helpers::validate_json(schema_json.value(), config_json.value());
    if (validation_err) {
        return "Config file validation failed: \n\t" + validation_err.value();
    }
    const auto &account_array = config_json->as_object().at("accounts").as_array();
    for (const auto &item : account_array) {
        const boost::json::object &account_json = item.as_object();
        evmc::address account_address;
        get_bytes(account_json.at("address"), account_address);
        account_storage.emplace(account_address, evmc::account());
        handle_account(account_json, account_storage.at(account_address));
    }
    return {};
}

std::optional<std::string> init_account_storage(evmc::accounts &account_storage,
                                                const std::string &account_storage_config_name) {
    std::ifstream asc_stream(account_storage_config_name);
    if (!asc_stream.is_open()) {
        return "Could not open the account storage config: '" + account_storage_config_name + "'";
    }
    return init_account_storage(account_storage, asc_stream);
}
