#include "zkevm_framework/data_types/transaction_receipt.hpp"

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    bytes TransactionReceipt::serialize() const {
        return ssz::serialize<TransactionReceipt::Serializable>(*this);
    }

    TransactionReceipt TransactionReceipt::deserialize(const bytes& src) {
        return ssz::deserialize<TransactionReceipt::Serializable>(src);
    }
}  // namespace data_types
