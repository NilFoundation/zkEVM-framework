#include "wrapper.hpp"

#include <nil/crypto3/algebra/curves/pallas.hpp>

#include "zkevm_framework/assigner_runner/runner.hpp"
#include "zkevm_framework/block_generator/block_generator.hpp"
#include "zkevm_framework/json_helpers/json_helpers.hpp"

using BlueprintFieldType = typename nil::crypto3::algebra::curves::pallas::base_field_type;

#ifdef __cplusplus
extern "C" {
#endif

void* make_runner() {
    const std::vector<std::array<std::size_t, 4>> column_sizes = {
        {65, 1, 1, 30},
        {65, 1, 1, 30},
    };
    return (void*)(new single_thread_runner<BlueprintFieldType>(column_sizes));
}

int run(void* handler, unsigned long shardId, const char* blockHash, const char* block_file_name) {
    single_thread_runner<BlueprintFieldType>* ptr =
        reinterpret_cast<single_thread_runner<BlueprintFieldType>*>(handler);
    return ptr->run("", std::nullopt) ? 1 : 0;
}

#ifdef __cplusplus
}
#endif
