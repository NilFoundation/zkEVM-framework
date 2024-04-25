#include "zkevm_framework/data_types/block_header.hpp"

#include "containers/block_header.hpp"
#include "utils.hpp"

namespace data_types {
    DEFAULT_IMPL_SERDE_METHODS(BlockHeader, containers::BlockHeaderContainer)
}  // namespace data_types
