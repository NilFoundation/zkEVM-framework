#include "zkevm_framework/data_types/transaction_receipt.hpp"

#include <iostream>
#include <optional>

#include "containers/transaction_receipt.hpp"
#include "sszpp/ssz++.hpp"
#include "utils.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    using namespace containers;

    bytes TransactionReceipt::serialize() const {
        return ssz::serialize<TransactionReceiptContainer>(*this);
    }

    int TransactionReceipt::serialize(std::ostream& out) const {
        return write_bytes(serialize(), out);
    }

    TransactionReceipt TransactionReceipt::deserialize(const bytes& src) {
        return static_cast<TransactionReceipt>(ssz::deserialize<TransactionReceiptContainer>(src));
    }

    std::optional<TransactionReceipt> TransactionReceipt::deserialize(std::istream& src) {
        return deserialize_from_stream<TransactionReceipt>(src);
    }
}  // namespace data_types
