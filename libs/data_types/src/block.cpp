#include "zkevm_framework/data_types/block.hpp"

#include "zkevm_framework/data_types/account_block.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/transaction.hpp"
#include "zkevm_framework/data_types/transaction_receipt.hpp"

namespace data_types {
    Block::Serializable::Serializable(const Block& block)
        : m_previousBlock(block.m_previousBlock), m_currentBlock(block.m_currentBlock) {
        for (const AccountBlock& account_block : block.m_accountBlocks) {
            m_accountBlocks.push_back(AccountBlock::Serializable(account_block));
        }
        for (const InMsg& in_msg : block.m_inputMsgs) {
            m_inputMsgs.push_back(InMsg::Serializable(in_msg));
        }
        for (const OutMsg& out_msg : block.m_outputMsgs) {
            m_outputMsgs.push_back(OutMsg::Serializable(out_msg));
        }
    }

    Block::Block(const Block::Serializable& s)
        : m_previousBlock(s.m_previousBlock), m_currentBlock(s.m_currentBlock) {
        for (const AccountBlock::Serializable& account_block : s.m_accountBlocks) {
            m_accountBlocks.push_back(AccountBlock(account_block));
        }
        for (const InMsg& in_msg : s.m_inputMsgs) {
            m_inputMsgs.push_back(InMsg(in_msg));
        }
        for (const OutMsg& out_msg : s.m_outputMsgs) {
            m_outputMsgs.push_back(OutMsg(out_msg));
        }
    }

    bytes Block::serialize() const { return ssz::serialize<Block::Serializable>(*this); }

    Block Block::deserialize(const bytes& src) {
        return ssz::deserialize<Block::Serializable>(src);
    }
}  // namespace data_types
