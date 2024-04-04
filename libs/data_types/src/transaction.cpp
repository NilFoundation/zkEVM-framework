#include "zkevm_framework/data_types/transaction.hpp"

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    bytes Transaction::serialize() const {
        return ssz::serialize<Transaction::Serializable>(*this);
    }

    Transaction Transaction::deserialize(const bytes &src) {
        return ssz::deserialize<Transaction::Serializable>(src);
    }
}  // namespace data_types
