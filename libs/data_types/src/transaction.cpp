#include "zkevm_framework/data_types/transaction.hpp"

#include "containers/transaction.hpp"
#include "utils.hpp"

namespace data_types {
    DEFAULT_IMPL_SERDE_METHODS(Transaction, containers::TransactionContainer)
}  // namespace data_types
