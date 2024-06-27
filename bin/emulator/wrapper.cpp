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
        {15, 1, 1, 30},
        {15, 1, 1, 30},
    };
    return (void*)(new single_thread_runner<BlueprintFieldType>(column_sizes));
}

int run(void* handler, unsigned long shardId, unsigned long blockId) {
    single_thread_runner<BlueprintFieldType>* ptr =
        reinterpret_cast<single_thread_runner<BlueprintFieldType>*>(handler);
    return ptr->run(shardId, blockId, "", std::nullopt) ? 1 : 0;
}

#ifdef __cplusplus
}
#endif
