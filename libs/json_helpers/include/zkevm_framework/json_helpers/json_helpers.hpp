#ifndef ZKEMV_FRAMEWORK_LIBS_JSON_HELPERS_INCLUDE_ZKEVM_FRAMEWORK_JSON_HELPERS_JSON_HELPERS_HPP_
#define ZKEMV_FRAMEWORK_LIBS_JSON_HELPERS_INCLUDE_ZKEVM_FRAMEWORK_JSON_HELPERS_JSON_HELPERS_HPP_

#include <boost/json/value.hpp>
#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <vector>

namespace json_helpers {

    /// @brief JSON Schema validator
    /// @returns the first found error if exists
    std::optional<std::string> validate_json(const boost::json::value &schema_json,
                                             const boost::json::value &target_json) noexcept;

    /// @brief Parse config file and transform it to Boost json representation
    std::expected<boost::json::value, std::string> parse_json(std::istream &stream) noexcept;

    /// @brief Extract unsigned number from value
    size_t get_number(const boost::json::value &json_value) noexcept;

    /// @brief Get vector of bytes from hex string
    std::vector<uint8_t> get_bytes(const boost::json::value &json_value) noexcept;
}  // namespace json_helpers

#endif  // ZKEMV_FRAMEWORK_LIBS_JSON_HELPERS_INCLUDE_ZKEVM_FRAMEWORK_JSON_HELPERS_JSON_HELPERS_HPP_
