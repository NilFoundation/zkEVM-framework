#include "zkevm_framework/data_types/account.hpp"

#include "containers/account.hpp"
#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    using namespace containers;

    bytes Account::serialize() const { return ssz::serialize<AccountContainer>(*this); }

    Account Account::deserialize(const bytes& src) {
        return static_cast<Account>(ssz::deserialize<AccountContainer>(src));
    }
}  // namespace data_types
