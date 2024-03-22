#include "zkevm_framework/data_types/block_header.hpp"

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    bytes BlockHeader::serialize() const {
        return ssz::serialize<BlockHeader::Serializable>(*this);
    }

    BlockHeader BlockHeader::deserialize(const bytes& src) {
        return ssz::deserialize<BlockHeader::Serializable>(src);
    }
}  // namespace data_types
