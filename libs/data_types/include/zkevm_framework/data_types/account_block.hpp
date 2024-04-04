/**
 * @file account_block.hpp
 * @brief This file defines Account Block.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ACCOUNT_BLOCK_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ACCOUNT_BLOCK_HPP_

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/mpt.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

namespace data_types {
    /// @brief Account block.
    class AccountBlock {
      public:
        friend class Block;

        Address m_accountAddress;
        MPTNode<Transaction> m_transactions;

        AccountBlock(Address accountAddress, MPTNode<Transaction> &transactions)
            : m_accountAddress(accountAddress), m_transactions(transactions) {}

        /// @returns the SSZ serialisation
        bytes serialize() const;

        /// @brief deserizalize from SSZ
        static AccountBlock deserialize(const bytes &src);

      private:
        struct Serializable : ssz::ssz_variable_size_container {
            Address m_accountAddress;
            ssz::list<Transaction::Serializable, 100> m_transactions;

            SSZ_CONT(m_accountAddress, m_transactions)

            Serializable() {}

            Serializable(const AccountBlock &account_block);
        };

        AccountBlock(const Serializable &s);
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ACCOUNT_BLOCK_HPP_
