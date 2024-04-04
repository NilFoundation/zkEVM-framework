#include "zkevm_framework/data_types/account_block.hpp"

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

namespace data_types {
    AccountBlock::Serializable::Serializable(const AccountBlock& account_block)
        : m_accountAddress(account_block.m_accountAddress) {
        for (const Transaction& transaction : account_block.m_transactions) {
            m_transactions.push_back(Transaction::Serializable(transaction));
        }
    }

    AccountBlock::AccountBlock(const Serializable& s) : m_accountAddress(s.m_accountAddress) {
        for (const Transaction::Serializable& transaction : s.m_transactions) {
            m_transactions.push_back(Transaction(transaction));
        }
    }

    bytes AccountBlock::serialize() const {
        return ssz::serialize<AccountBlock::Serializable>(*this);
    }

    AccountBlock AccountBlock::deserialize(const bytes& src) {
        return ssz::deserialize<AccountBlock::Serializable>(src);
    }
}  // namespace data_types
