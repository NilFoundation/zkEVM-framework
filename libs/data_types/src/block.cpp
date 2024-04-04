#include "zkevm_framework/data_types/block.hpp"

#include "zkevm_framework/data_types/account_block.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/transaction.hpp"
#include "zkevm_framework/data_types/transaction_receipt.hpp"

namespace data_types {
    Block::Serializable::Serializable(const Block& block)
        : m_previousBlock(block.m_previousBlock),
          m_currentBlock(block.m_currentBlock),
          m_inputMsg(block.m_inputMsg),
          m_outputMsg(block.m_outputMsg) {
        for (const AccountBlock& account_block : block.m_accountBlocks) {
            m_accountBlocks.push_back(AccountBlock::Serializable(account_block));
        }
    }

    Block::Block(const Block::Serializable& s)
        : m_previousBlock(s.m_previousBlock),
          m_currentBlock(s.m_currentBlock),
          m_inputMsg(s.m_inputMsg),
          m_outputMsg(s.m_outputMsg) {
        for (const AccountBlock::Serializable& account_block : s.m_accountBlocks) {
            m_accountBlocks.push_back(AccountBlock(account_block));
        }
    }

    bytes Block::serialize() const { return ssz::serialize<Block::Serializable>(*this); }

    Block Block::deserialize(const bytes& src) {
        return ssz::deserialize<Block::Serializable>(src);
    }
}  // namespace data_types
