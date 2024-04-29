#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_TRANSACTION_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_TRANSACTION_HPP_

#include <cstddef>
#include <cstdint>

#include "ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

namespace data_types {
    namespace containers {
        struct TransactionContainer : ssz::ssz_variable_size_container {
            size_t m_id;
            uint8_t m_type;
            size_t m_nonce;
            size_t m_value;
            Address m_receiveAddress;
            size_t m_gasPrice;
            size_t m_gas;
            ssz::list<std::byte, 100> m_data;
            Address m_sender;

            SSZ_CONT(m_id, m_type, m_nonce, m_value, m_receiveAddress, m_gasPrice, m_gas, m_data,
                     m_sender)

            TransactionContainer() {}

            TransactionContainer(const Transaction& transaction)
                : m_id(transaction.m_id),
                  m_nonce(transaction.m_nonce),
                  m_value(transaction.m_value),
                  m_receiveAddress(transaction.m_receiveAddress),
                  m_gasPrice(transaction.m_gasPrice),
                  m_gas(transaction.m_gas),
                  m_sender(transaction.m_sender),
                  m_type(transaction.m_type),
                  m_data(transaction.m_data) {}

            operator Transaction() const {
                return Transaction(m_id, static_cast<Transaction::Type>(m_type), m_nonce, m_value,
                                   m_receiveAddress, m_gasPrice, m_gas, m_data.data(), m_sender);
            }
        };
    }  // namespace containers
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_TRANSACTION_HPP_
