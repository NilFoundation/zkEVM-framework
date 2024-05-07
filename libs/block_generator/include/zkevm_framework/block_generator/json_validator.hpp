#ifndef ZKEMV_FRAMEWORK_LIBS_BLOCK_GENERATOR_JSON_VALIDATOR_HPP_
#define ZKEMV_FRAMEWORK_LIBS_BLOCK_GENERATOR_JSON_VALIDATOR_HPP_

#include <optional>
#include <sstream>
#include <string>
#include <valijson/adapters/boost_json_adapter.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>

/// @brief Inline json validator
/// @returns the first found error if exists
inline std::optional<std::string> validate_json(const boost::json::value &schema_json,
                                                const boost::json::value &target_json) {
    valijson::Schema schema;
    valijson::SchemaParser schemaParser;

    valijson::adapters::BoostJsonAdapter schemaAdapter(schema_json);

#if VALIJSON_USE_EXCEPTIONS
    try {
#endif
        schemaParser.populateSchema(schemaAdapter, schema);
#if VALIJSON_USE_EXCEPTIONS
    } catch (std::exception &error)) {
        return "Schema error: " + error.what();
    }
#endif

    valijson::Validator validator;
    valijson::ValidationResults results;
    valijson::adapters::BoostJsonAdapter targetAdapter(target_json);

    if (validator.validate(schema, targetAdapter, &results)) {
        // Success
        return {};
    }
    valijson::ValidationResults::Error error;
    results.popError(error);
    std::ostringstream error_stream;
    error_stream << error.description << " Location: ";
    for (const auto &location : error.context) {
        error_stream << location;
    }
    return error_stream.str();
}

#endif  // ZKEMV_FRAMEWORK_LIBS_BLOCK_GENERATOR_JSON_VALIDATOR_HPP_
