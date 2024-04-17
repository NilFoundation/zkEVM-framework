/**
 * @file transaction_receipt.hpp
 * @brief This file defines Transaction receipt.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_RECEIPT_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_RECEIPT_HPP_

#include <cstdint>

#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

namespace data_types {
    /// @brief Transaction receipt.
    class TransactionReceipt {
      public:
        friend class Block;

        Transaction::Type m_type;
        /// @brief Root hash of the state after this transaction
        Hash m_stateRoot;
        size_t m_gasUsed;

        TransactionReceipt(Transaction::Type type, Hash root, size_t gasUsed)
            : m_type(type), m_stateRoot(root), m_gasUsed(gasUsed) {}

        /// @returns the SSZ serialization of transaction receipt
        bytes serialize() const;

        /// @brief deserizalize from SSZ
        static TransactionReceipt deserialize(const bytes& src);
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_RECEIPT_HPP_
