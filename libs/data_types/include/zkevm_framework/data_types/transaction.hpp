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
        friend class Block;

        /// @brief Transaction type
        enum Type : uint8_t { NullTransaction, ContractCreation, MessageCall };

        /// @brief empty transaction
        Transaction() {}

        /// @brief an unsigned contract-creation transaction
        Transaction(size_t value, size_t gasPrice, size_t gas, bytes const& data, size_t nonce)
            : m_value(value),
              m_gasPrice(gasPrice),
              m_gas(gas),
              m_data(data),
              m_nonce(nonce),
              m_type(ContractCreation),
              m_receiveAddress{} {}

        /// @returns sender of the transaction
        Address const& sender() const { return m_sender; }

        /// @brief Set sender
        void forceSender(Address const& a) { m_sender = a; }

        /// @returns true if transaction is contract-creation
        bool isCreation() const { return m_type == ContractCreation; }

        /// @returns the amount of ETH to be transferred by this (message-call)
        /// transaction
        size_t value() const { return m_value; }

        /// @returns the gas price
        size_t gasPrice() const { return m_gasPrice; }

        /// @returns the total gas
        size_t gas() const { return m_gas; }

        /// @returns the receiving address
        Address to() const { return m_receiveAddress; }

        /// @returns the sender address
        Address from() const { return m_sender; }

        /// @returns the data associated with this transaction. Synonym for initCode()
        bytes const& data() const { return m_data; }

        /// @returns the transaction-count of the sender
        size_t nonce() const { return m_nonce; }

        /// @returns the SSZ serialization of this transaction
        bytes serialize() const;

        /// @brief deserizalize from SSZ
        static Transaction deserialize(bytes src);

        /// @returns merkalization root of SSZ serialization of this transaction
        SSZHash hash_tree_root() const;

        /// @returns the hash of the SSZ serialization of this transaction
        Hash hash() const { return 0; }  // TODO: implement.

      private:
        Type m_type;
        size_t m_nonce;
        size_t m_value;
        Address m_receiveAddress;
        size_t m_gasPrice;
        size_t m_gas;
        bytes m_data;  // constructorCode + contractCode + auxdata +
                       // constructorData
        Address m_sender;

        /// @brief Serializable mirroring structure of `Transaction`.
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
