#include "zkevm_framework/data_types/transaction_receipt.hpp"

#include "containers/transaction_receipt.hpp"
#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    using namespace containers;

    bytes TransactionReceipt::serialize() const {
        return ssz::serialize<TransactionReceiptContainer>(*this);
    }

    TransactionReceipt TransactionReceipt::deserialize(const bytes& src) {
        return static_cast<TransactionReceipt>(ssz::deserialize<TransactionReceiptContainer>(src));
    }
}  // namespace data_types
