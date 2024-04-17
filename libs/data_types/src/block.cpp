#include "zkevm_framework/data_types/block.hpp"

#include "containers/block.hpp"
#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    using namespace containers;

    bytes Block::serialize() const { return ssz::serialize<BlockContainer>(*this); }

    Block Block::deserialize(const bytes& src) {
        return static_cast<Block>(ssz::deserialize<BlockContainer>(src));
    }
}  // namespace data_types
