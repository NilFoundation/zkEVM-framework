#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_BLOCK_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_BLOCK_HPP_

#include "containers/account_block.hpp"
#include "containers/block_header.hpp"
#include "containers/message.hpp"
#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/account_block.hpp"
#include "zkevm_framework/data_types/block.hpp"
#include "zkevm_framework/data_types/message.hpp"
#include "zkevm_framework/data_types/mpt.hpp"

namespace data_types {
    namespace containers {
        struct BlockContainer : ssz::ssz_variable_size_container {
            ssz::list<AccountBlockContainer, 100> m_accountBlocks;
            ssz::list<InMsgContainer, 100> m_inputMsgs;
            ssz::list<OutMsgContainer, 100> m_outputMsgs;
            BlockHeaderContainer m_previousBlock;
            BlockHeaderContainer m_currentBlock;

            SSZ_CONT(m_accountBlocks, m_inputMsgs, m_outputMsgs, m_previousBlock, m_currentBlock)

            BlockContainer() {}

            BlockContainer(const Block& block)
                : m_previousBlock(block.m_previousBlock), m_currentBlock(block.m_currentBlock) {
                for (const AccountBlock& account_block : block.m_accountBlocks) {
                    m_accountBlocks.push_back(AccountBlockContainer(account_block));
                }
                for (const InMsg& in_msg : block.m_inputMsgs) {
                    m_inputMsgs.push_back(InMsgContainer(in_msg));
                }
                for (const OutMsg& out_msg : block.m_outputMsgs) {
                    m_outputMsgs.push_back(OutMsgContainer(out_msg));
                }
            }

            operator Block() const {
                MPTNode<AccountBlock> accountBlocks;
                for (const AccountBlockContainer& account_block : m_accountBlocks) {
                    accountBlocks.push_back(static_cast<AccountBlock>(account_block));
                }
                MPTNode<InMsg> inputMsgs;
                for (const InMsgContainer& in_msg : m_inputMsgs) {
                    inputMsgs.push_back(static_cast<InMsg>(in_msg));
                }
                MPTNode<OutMsg> outputMsgs;
                for (const OutMsgContainer& out_msg : m_outputMsgs) {
                    outputMsgs.push_back(static_cast<OutMsg>(out_msg));
                }
                return Block(accountBlocks, inputMsgs, outputMsgs,
                             static_cast<BlockHeader>(m_previousBlock),
                             static_cast<BlockHeader>(m_currentBlock));
            }
        };
    }  // namespace containers
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_BLOCK_HPP_
