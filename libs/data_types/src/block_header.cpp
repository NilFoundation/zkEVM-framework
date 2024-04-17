#include "zkevm_framework/data_types/block_header.hpp"

#include "containers/block_header.hpp"
#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    using namespace containers;

    bytes BlockHeader::serialize() const { return ssz::serialize<BlockHeaderContainer>(*this); }

    BlockHeader BlockHeader::deserialize(const bytes& src) {
        return static_cast<BlockHeader>(ssz::deserialize<BlockHeaderContainer>(src));
    }
}  // namespace data_types
