#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_ACCOUNT_BLOCK_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_ACCOUNT_BLOCK_HPP_

#include "containers/transaction.hpp"
#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/account_block.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/mpt.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

namespace data_types {
    namespace containers {
        struct AccountBlockContainer : ssz::ssz_variable_size_container {
            Address m_accountAddress;
            ssz::list<TransactionContainer, 100> m_transactions;

            SSZ_CONT(m_accountAddress, m_transactions)

            AccountBlockContainer() {}

            AccountBlockContainer(const AccountBlock& account_block)
                : m_accountAddress(account_block.m_accountAddress) {
                for (const Transaction& transaction : account_block.m_transactions) {
                    m_transactions.push_back(TransactionContainer(transaction));
                }
            }

            operator AccountBlock() const {
                MPTNode<Transaction> transactions;
                for (const TransactionContainer& transaction : m_transactions) {
                    transactions.push_back(static_cast<Transaction>(transaction));
                }
                return AccountBlock(m_accountAddress, transactions);
            }
        };
    }  // namespace containers
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_ACCOUNT_BLOCK_HPP_
