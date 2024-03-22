#include "zkevm_framework/data_types/block.hpp"

#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/transaction.hpp"
#include "zkevm_framework/data_types/transaction_receipt.hpp"

namespace data_types {
    Block::Serializable::Serializable(const Block& block)
        : m_previousBlock(block.m_previousBlock), m_currentBlock(block.m_currentBlock) {
        for (const Transaction& transaction : block.m_transactions) {
            m_transactions.push_back(Transaction::Serializable(transaction));
        }
        for (const TransactionReceipt& receipt : block.m_receipts) {
            m_receipts.push_back(TransactionReceipt::Serializable(receipt));
        }
    }

    Block::Block(const Block::Serializable& s)
        : m_previousBlock(s.m_previousBlock), m_currentBlock(s.m_currentBlock) {
        for (const Transaction::Serializable& transaction : s.m_transactions) {
            m_transactions.push_back(Transaction(transaction));
        }
        for (const TransactionReceipt::Serializable& receipt : s.m_receipts) {
            m_receipts.push_back(TransactionReceipt(receipt));
        }
    }

    bytes Block::serialize() const { return ssz::serialize<Block::Serializable>(*this); }

    Block Block::deserialize(const bytes& src) {
        return ssz::deserialize<Block::Serializable>(src);
    }
}  // namespace data_types
