#include <boost/algorithm/hex.hpp>
#include <sstream>
#include <valijson/adapters/boost_json_adapter.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>
#include <zkevm_framework/json_helpers/json_helpers.hpp>

namespace json_helpers {

    std::optional<std::string> validate_json(const boost::json::value &schema_json,
                                             const boost::json::value &target_json) noexcept {
        valijson::Schema schema;
        valijson::SchemaParser schemaParser;

        valijson::adapters::BoostJsonAdapter schemaAdapter(schema_json);

#if VALIJSON_USE_EXCEPTIONS
        try {
#endif
            schemaParser.populateSchema(schemaAdapter, schema);
#if VALIJSON_USE_EXCEPTIONS
        } catch (std::exception &error) {
            return "Schema error: " + std::string(error.what());
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

    std::expected<boost::json::value, std::string> parse_json(std::istream &stream) noexcept {
        boost::json::stream_parser p;
        boost::json::error_code ec;
        while (!stream.eof()) {
            char input_string[256];
            stream.read(input_string, sizeof(input_string) - 1);
            input_string[stream.gcount()] = '\0';
            p.write(input_string, ec);
            if (ec) {
                return std::unexpected<std::string>(ec.message());
            }
        }
        p.finish(ec);
        if (ec) {
            return std::unexpected<std::string>(ec.message());
        }
        return p.release();
    }

    size_t get_number(const boost::json::value &json_value) noexcept {
        if (json_value.is_int64()) {
            return json_value.as_int64();
        }
        return json_value.as_uint64();
    }

    std::vector<uint8_t> get_bytes(const boost::json::value &json_value) noexcept {
        std::string hex_string = json_value.as_string().c_str();
        std::vector<uint8_t> bytes;
        // Skip 0x prefix if exists
        if (hex_string[0] == '0' && hex_string[1] == 'x') {
            hex_string.erase(0, 2);
        }
        boost::algorithm::unhex(hex_string, std::back_inserter(bytes));
        return bytes;
    }
}  // namespace json_helpers
