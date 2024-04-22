#include "zkevm_framework/data_types/account_block.hpp"

#include <iostream>
#include <optional>

#include "containers/account_block.hpp"
#include "sszpp/ssz++.hpp"
#include "utils.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/mpt.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

namespace data_types {
    using namespace containers;

    bytes AccountBlock::serialize() const { return ssz::serialize<AccountBlockContainer>(*this); }

    int AccountBlock::serialize(std::ostream& out) const { return write_bytes(serialize(), out); }

    AccountBlock AccountBlock::deserialize(const bytes& src) {
        return static_cast<AccountBlock>(ssz::deserialize<AccountBlockContainer>(src));
    }

    std::optional<AccountBlock> AccountBlock::deserialize(std::istream& src) {
        return deserialize_from_stream<AccountBlock>(src);
    }
}  // namespace data_types
