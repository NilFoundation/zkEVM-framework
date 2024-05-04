/**
 * @file account_block.hpp
 * @brief This file defines Account Block.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ACCOUNT_BLOCK_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ACCOUNT_BLOCK_HPP_

#include <cstring>
#include <expected>
#include <iostream>

#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/errors.hpp"
#include "zkevm_framework/data_types/mpt.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

namespace data_types {
    /// @brief Account block.
    class AccountBlock {
      public:
        friend class Block;

        Address m_accountAddress;
        MPTNode<Transaction> m_transactions;

        AccountBlock(Address accountAddress, const MPTNode<Transaction>& transactions)
            : m_accountAddress(accountAddress), m_transactions(transactions) {}

        /// @brief Default constructor - zero initializer
        AccountBlock() { std::memset(m_accountAddress.data(), 0, m_accountAddress.size()); }

        /// @returns the SSZ serialisation
        std::expected<bytes, SerializationError> serialize() const noexcept;

        /// @brief write SSZ serialization to stream
        std::expected<void, SerializationError> serialize(std::ostream& out) const noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<AccountBlock, SerializationError> deserialize(
            const bytes& src) noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<AccountBlock, SerializationError> deserialize(
            std::istream& src) noexcept;
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ACCOUNT_BLOCK_HPP_
