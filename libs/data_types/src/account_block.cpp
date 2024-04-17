#include "zkevm_framework/data_types/account_block.hpp"

#include "containers/account_block.hpp"
#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/mpt.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

namespace data_types {
    using namespace containers;

    bytes AccountBlock::serialize() const { return ssz::serialize<AccountBlockContainer>(*this); }

    AccountBlock AccountBlock::deserialize(const bytes& src) {
        return static_cast<AccountBlock>(ssz::deserialize<AccountBlockContainer>(src));
    }
}  // namespace data_types
