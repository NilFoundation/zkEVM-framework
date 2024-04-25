/**
 * @file transaction.hpp
 * @brief This file defines Transaction.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_HPP_

#include <cstdint>
#include <expected>
#include <iostream>

#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/errors.hpp"

namespace data_types {
    /// @brief Transaction.
    class Transaction {
      public:
        friend class AccountBlock;
        friend class InMsg;
        friend class OutMsg;

        /// @brief Transaction type
        enum Type : uint8_t { NullTransaction, ContractCreation, MessageCall };

        size_t m_id;
        Type m_type;
        size_t m_nonce;
        size_t m_value;
        Address m_receiveAddress;
        size_t m_gasPrice;
        size_t m_gas;
        bytes m_data;  // constructorCode + contractCode + auxdata + constructorData
        Address m_sender;

        /// @brief an unsigned contract-creation transaction
        Transaction(size_t id, Type type, size_t nonce, size_t value, Address receiveAddress,
                    size_t gasPrice, size_t gas, bytes const& data, Address sender)
            : m_id(id),
              m_value(value),
              m_gasPrice(gasPrice),
              m_gas(gas),
              m_data(data),
              m_nonce(nonce),
              m_type(type),
              m_receiveAddress(receiveAddress),
              m_sender(sender) {}

        /// @returns the SSZ serialization of this transaction
        std::expected<bytes, SerializationError> serialize() const noexcept;

        /// @brief write SSZ serialization to stream
        std::expected<void, SerializationError> serialize(std::ostream& out) const noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<Transaction, SerializationError> deserialize(
            const bytes& src) noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<Transaction, SerializationError> deserialize(
            std::istream& src) noexcept;
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_TRANSACTION_HPP_
