#include "wrapper.hpp"

#include "runner.hpp"
#include "zkevm_framework/block_generator/block_generator.hpp"
#include "zkevm_framework/json_helpers/json_helpers.hpp"

using BlueprintFieldType = typename nil::crypto3::algebra::curves::pallas::base_field_type;

#ifdef __cplusplus
extern "C" {
#endif

void* make_runner() {
    const std::vector<std::array<std::size_t, 4>> column_sizes = {
        {15, 1, 1, 30},
        {15, 1, 1, 30},
    };
    return (void*)(new single_thread_runner<BlueprintFieldType>(column_sizes));
}

int run(void* handler, const unsigned char* input_block_data, unsigned long input_block_size) {
    single_thread_runner<BlueprintFieldType>* ptr =
        reinterpret_cast<single_thread_runner<BlueprintFieldType>*>(handler);
    return ptr->run(input_block_data, input_block_size, "", std::nullopt);
}

int create_block(const char* config_file_name, unsigned char* input_block_data) {
    std::ifstream config_file(config_file_name);
    if (!config_file.is_open()) {
        return -1;
    }

    std::expected<boost::json::value, std::string> parsed_json =
        json_helpers::parse_json(config_file);
    if (!parsed_json) {
        return -1;
    }

    std::expected<data_types::Block, std::string> block =
        data_types::block_generator::generate_block(parsed_json.value());
    if (!block) {
        return -1;
    }

    // TODO: switch to std::ostream
    std::expected<data_types::bytes, data_types::SerializationError> serialization_result =
        block->serialize();
    if (!serialization_result) {
        return -1;
    }
    data_types::bytes serialized_data = serialization_result.value();
    std::memcpy(input_block_data, serialized_data.data(), serialized_data.size());
    return serialized_data.size();
}

#ifdef __cplusplus
}
#endif
