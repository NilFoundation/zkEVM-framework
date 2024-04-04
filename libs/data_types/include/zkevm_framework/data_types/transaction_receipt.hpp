/**
 * @file transaction_receipt.hpp
 * @brief This file defines Transaction receipt.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_RECEIPT_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_RECEIPT_HPP_

#include <cstdint>

#include "sszpp/ssz++.hpp"
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

      private:
        struct Serializable : ssz::ssz_container {
            uint8_t m_type;
            Hash m_stateRoot;
            size_t m_gasUsed;

            SSZ_CONT(m_type, m_stateRoot, m_gasUsed)

            Serializable() {}

            Serializable(const TransactionReceipt& receipt)
                : m_type(receipt.m_type),
                  m_stateRoot(receipt.m_stateRoot),
                  m_gasUsed(receipt.m_gasUsed) {}
        };

        TransactionReceipt(const Serializable& s)
            : m_type(static_cast<Transaction::Type>(s.m_type)),
              m_stateRoot(s.m_stateRoot),
              m_gasUsed(s.m_gasUsed) {}
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_RECEIPT_HPP_
