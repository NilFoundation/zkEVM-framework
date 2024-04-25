#include "zkevm_framework/data_types/transaction_receipt.hpp"

#include "containers/transaction_receipt.hpp"
#include "utils.hpp"

namespace data_types {
    DEFAULT_IMPL_SERDE_METHODS(TransactionReceipt, containers::TransactionReceiptContainer)
}  // namespace data_types
