#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_RECEIPT_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_RECEIPT_HPP_

#include <cstdint>

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

namespace data_types {
    class TransactionReceipt {
      public:
        friend class Block;

        TransactionReceipt() {}

        TransactionReceipt(Transaction::Type type, Hash _root, size_t gasUsed)
            : m_type(type), m_stateRoot(_root), m_gasUsed(gasUsed) {}

        Transaction::Type getType() const { return m_type; }
        Hash const& stateRoot() const { return m_stateRoot; }
        size_t const& gasUsed() const { return m_gasUsed; }

        /// @returns the SSZ serialization of transaction receipt
        bytes serialize() const;

        /// @brief deserizalize from SSZ
        static TransactionReceipt deserialize(const bytes& src);

      private:
        Transaction::Type m_type;
        /// Root hash of the state after this transaction
        Hash m_stateRoot;
        size_t m_gasUsed;

        /// @brief Serializable mirroring structure of `TransactionReceipt`.
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
