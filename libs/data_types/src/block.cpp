#include "zkevm_framework/data_types/block.hpp"

#include <iostream>
#include <optional>

#include "containers/block.hpp"
#include "sszpp/ssz++.hpp"
#include "utils.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    using namespace containers;

    bytes Block::serialize() const { return ssz::serialize<BlockContainer>(*this); }

    int Block::serialize(std::ostream& out) const { return write_bytes(serialize(), out); }

    Block Block::deserialize(const bytes& src) {
        return static_cast<Block>(ssz::deserialize<BlockContainer>(src));
    }

    std::optional<Block> Block::deserialize(std::istream& src) {
        return deserialize_from_stream<Block>(src);
    }
}  // namespace data_types
