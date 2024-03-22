#include "zkevm_framework/data_types/account.hpp"

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    bytes Account::serialize() const { return ssz::serialize<Account::Serializable>(*this); }

    Account Account::deserialize(const bytes& src) {
        return ssz::deserialize<Account::Serializable>(src);
    }
}  // namespace data_types
