#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_TRANSACTION_RECEIPT_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_TRANSACTION_RECEIPT_HPP_

#include <cstdint>

#include "ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/transaction.hpp"
#include "zkevm_framework/data_types/transaction_receipt.hpp"

namespace data_types {
    namespace containers {
        struct TransactionReceiptContainer : ssz::ssz_container {
            uint8_t m_type;
            Hash m_stateRoot;
            size_t m_gasUsed;

            SSZ_CONT(m_type, m_stateRoot, m_gasUsed)

            TransactionReceiptContainer() {}

            TransactionReceiptContainer(const TransactionReceipt& receipt)
                : m_type(receipt.m_type),
                  m_stateRoot(receipt.m_stateRoot),
                  m_gasUsed(receipt.m_gasUsed) {}

            operator TransactionReceipt() const {
                return TransactionReceipt(static_cast<Transaction::Type>(m_type), m_stateRoot,
                                          m_gasUsed);
            }
        };
    }  // namespace containers
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_TRANSACTION_RECEIPT_HPP_
