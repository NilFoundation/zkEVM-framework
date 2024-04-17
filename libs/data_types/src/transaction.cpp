#include "zkevm_framework/data_types/transaction.hpp"

#include "containers/transaction.hpp"
#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    using namespace containers;

    bytes Transaction::serialize() const { return ssz::serialize<TransactionContainer>(*this); }

    Transaction Transaction::deserialize(const bytes &src) {
        return static_cast<Transaction>(ssz::deserialize<TransactionContainer>(src));
    }
}  // namespace data_types
