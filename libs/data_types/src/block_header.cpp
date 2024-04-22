#include "zkevm_framework/data_types/block_header.hpp"

#include <iostream>
#include <optional>

#include "containers/block_header.hpp"
#include "sszpp/ssz++.hpp"
#include "utils.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    using namespace containers;

    bytes BlockHeader::serialize() const { return ssz::serialize<BlockHeaderContainer>(*this); }

    int BlockHeader::serialize(std::ostream& out) const { return write_bytes(serialize(), out); }

    BlockHeader BlockHeader::deserialize(const bytes& src) {
        return static_cast<BlockHeader>(ssz::deserialize<BlockHeaderContainer>(src));
    }

    std::optional<BlockHeader> BlockHeader::deserialize(std::istream& src) {
        return deserialize_from_stream<BlockHeader>(src);
    }
}  // namespace data_types
