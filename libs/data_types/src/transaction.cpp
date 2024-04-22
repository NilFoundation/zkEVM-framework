#include "zkevm_framework/data_types/transaction.hpp"

#include <iostream>
#include <optional>

#include "containers/transaction.hpp"
#include "sszpp/ssz++.hpp"
#include "utils.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    using namespace containers;

    bytes Transaction::serialize() const { return ssz::serialize<TransactionContainer>(*this); }

    int Transaction::serialize(std::ostream& out) const { return write_bytes(serialize(), out); }

    Transaction Transaction::deserialize(const bytes& src) {
        return static_cast<Transaction>(ssz::deserialize<TransactionContainer>(src));
    }

    std::optional<Transaction> Transaction::deserialize(std::istream& src) {
        return deserialize_from_stream<Transaction>(src);
    }
}  // namespace data_types
