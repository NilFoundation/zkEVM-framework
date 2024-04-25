/**
 * @file transaction_receipt.hpp
 * @brief This file defines Transaction receipt.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_RECEIPT_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_RECEIPT_HPP_

#include <cstdint>
#include <expected>
#include <iostream>

#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/errors.hpp"
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
        std::expected<bytes, SerializationError> serialize() const noexcept;

        /// @brief write SSZ serialization to stream
        std::expected<void, SerializationError> serialize(std::ostream& out) const noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<TransactionReceipt, SerializationError> deserialize(
            const bytes& src) noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<TransactionReceipt, SerializationError> deserialize(
            std::istream& src) noexcept;
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_RECEIPT_HPP_
