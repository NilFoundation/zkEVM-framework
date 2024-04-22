#include "zkevm_framework/data_types/account.hpp"

#include <iostream>
#include <optional>

#include "containers/account.hpp"
#include "sszpp/ssz++.hpp"
#include "utils.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    using namespace containers;

    bytes Account::serialize() const { return ssz::serialize<AccountContainer>(*this); }

    int Account::serialize(std::ostream& out) const { return write_bytes(serialize(), out); }

    Account Account::deserialize(const bytes& src) {
        return static_cast<Account>(ssz::deserialize<AccountContainer>(src));
    }

    std::optional<Account> Account::deserialize(std::istream& src) {
        return deserialize_from_stream<Account>(src);
    }
}  // namespace data_types
