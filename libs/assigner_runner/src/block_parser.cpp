#include "zkevm_framework/assigner_runner/block_parser.hpp"

#include <boost/algorithm/hex.hpp>
#include <boost/endian.hpp>
#include <boost/endian/conversion.hpp>
#include <cstring>
#include <fstream>
#include <iostream>

#include "block_schema.def"
#include "zkevm_framework/json_helpers/json_helpers.hpp"

template<typename T>
std::optional<std::string> to_std_bytes(const boost::json::value &json_value, T &dst) noexcept {
    std::vector<uint8_t> bytes;
    json_helpers::to_bytes(json_value, bytes);
    for (std::size_t i = 0; i < bytes.size(); i++) {
        dst[i] = std::byte{bytes[i]};
    }
    return std::nullopt;
}

static std::optional<std::string> handle_message(const boost::json::object &val,
                                                 core::types::Message &message) {
    const auto &json_flags = val.at("flags").as_array();
    for (const auto &flag : json_flags) {
        const std::string flag_str = flag.as_string().c_str();
        if (flag_str == "Internal") {
            message.m_flags.set(std::size_t(core::types::MessageKind::Internal));
        } else if (flag_str == "Deploy") {
            message.m_flags.set(std::size_t(core::types::MessageKind::Deploy));
        } else if (flag_str == "Refund") {
            message.m_flags.set(std::size_t(core::types::MessageKind::Refund));
        } else if (flag_str == "Bounce") {
            message.m_flags.set(std::size_t(core::types::MessageKind::Bounce));
        }
    }
    to_std_bytes<core::types::Address>(val.at("from"), message.m_from);
    to_std_bytes<core::types::Address>(val.at("to"), message.m_to);
    std::string value_str = val.at("value").as_string().c_str();
    message.m_value.m_value = intx::from_string<intx::uint256>(value_str);
    std::vector<std::byte> data_bytes;
    json_helpers::to_std_bytes(val.at("data"), data_bytes);
    message.m_data.data() = data_bytes;
    return {};
}

static std::optional<std::string> handle_block(const boost::json::object &val,
                                               core::types::Block &block,
                                               std::vector<core::types::Message> &messages) {
    block.m_id = val.at("number").as_int64();
    to_std_bytes<core::Hash>(val.at("parentHash"), block.m_prev_block);

    const auto &json_input_msgs = val.at("messages").as_array();
    for (const auto &msg : json_input_msgs) {
        core::types::Message message;
        const boost::json::object &msg_json = msg.as_object();
        if (auto parse_err = handle_message(msg_json, message)) {
            return parse_err.value();
        }
        messages.push_back(message);
    }
    return {};
}

std::optional<std::string> load_block_with_messages(core::types::Block &block,
                                                    std::vector<core::types::Message> &messages,
                                                    std::istream &block_data) {
    auto block_json = json_helpers::parse_json(block_data);
    if (!block_json) {
        return "Error while parsing block file: " + block_json.error();
    }
    std::stringstream schema_stream;
    schema_stream << block_schema;
    auto schema_json = json_helpers::parse_json(schema_stream);
    if (!schema_json) {
        return "Error while parsing block schema: " + schema_json.error();
    }
    auto validation_err = json_helpers::validate_json(schema_json.value(), block_json.value());
    if (validation_err) {
        return "Block file validation failed: \n\t" + validation_err.value();
    }

    auto parse_block_err = handle_block(block_json.value().as_object(), block, messages);
    if (parse_block_err) {
        return "Parse block failed: \n\t" + parse_block_err.value();
    }
    return {};
}
