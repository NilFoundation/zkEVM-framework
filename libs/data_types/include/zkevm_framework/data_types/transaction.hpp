/**
 * @file transaction.hpp
 * @brief This file defines Transaction.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_HPP_

#include <cstdint>

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    /// @brief Transaction.
    class Transaction {
      public:
        friend class AccountBlock;

        /// @brief Transaction type
        enum Type : uint8_t { NullTransaction, ContractCreation, MessageCall };

        Type m_type;
        size_t m_nonce;
        size_t m_value;
        Address m_receiveAddress;
        size_t m_gasPrice;
        size_t m_gas;
        bytes m_data;  // constructorCode + contractCode + auxdata + constructorData
        Address m_sender;

        /// @brief an unsigned contract-creation transaction
        Transaction(Type type, size_t nonce, size_t value, Address receiveAddress, size_t gasPrice,
                    size_t gas, bytes const& data, Address sender)
            : m_value(value),
              m_gasPrice(gasPrice),
              m_gas(gas),
              m_data(data),
              m_nonce(nonce),
              m_type(type),
              m_receiveAddress(receiveAddress),
              m_sender(sender) {}

        /// @returns the SSZ serialization of this transaction
        bytes serialize() const;

        /// @brief deserizalize from SSZ
        static Transaction deserialize(const bytes& src);

      private:
        struct Serializable : ssz::ssz_variable_size_container {
            uint8_t m_type;
            size_t m_nonce;
            size_t m_value;
            Address m_receiveAddress;
            size_t m_gasPrice;
            size_t m_gas;
            ssz::list<std::byte, 100> m_data;
            Address m_sender;

            SSZ_CONT(m_type, m_nonce, m_value, m_receiveAddress, m_gasPrice, m_gas, m_data,
                     m_sender)

            Serializable() {}

            Serializable(const Transaction& transaction)
                : m_nonce(transaction.m_nonce),
                  m_value(transaction.m_value),
                  m_receiveAddress(transaction.m_receiveAddress),
                  m_gasPrice(transaction.m_gasPrice),
                  m_gas(transaction.m_gas),
                  m_sender(transaction.m_sender),
                  m_type(transaction.m_type),
                  m_data(transaction.m_data) {}
        };

        Transaction(const Serializable& s)
            : m_nonce(s.m_nonce),
              m_value(s.m_value),
              m_receiveAddress(s.m_receiveAddress),
              m_gasPrice(s.m_gasPrice),
              m_gas(s.m_gas),
              m_sender(s.m_sender),
              m_type(static_cast<Type>(s.m_type)),
              m_data(s.m_data.begin(), s.m_data.end()) {}
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_HPP_
