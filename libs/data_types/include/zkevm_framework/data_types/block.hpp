/**
 * @file block.hpp
 * @brief This file defines Block.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HPP_

#include "zkevm_framework/data_types/account_block.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/block_header.hpp"
#include "zkevm_framework/data_types/message.hpp"
#include "zkevm_framework/data_types/mpt.hpp"

namespace data_types {
    /// @brief Block.
    class Block {
      public:
        MPTNode<AccountBlock> m_accountBlocks;
        MPTNode<InMsg> m_inputMsgs;
        MPTNode<OutMsg> m_outputMsgs;
        BlockHeader m_previousBlock;
        BlockHeader m_currentBlock;

        Block(const MPTNode<AccountBlock> &accountBlocks, const MPTNode<InMsg> &inputMsgs,
              const MPTNode<OutMsg> &outputMsgs, const BlockHeader &previousBlock,
              const BlockHeader &currentBlock)
            : m_accountBlocks(accountBlocks),
              m_inputMsgs(inputMsgs),
              m_outputMsgs(outputMsgs),
              m_previousBlock(previousBlock),
              m_currentBlock(currentBlock) {}

        /// @returns the SSZ serialization
        bytes serialize() const;

        /// @brief deserizalize from SSZ
        static Block deserialize(const bytes &src);
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HPP_
