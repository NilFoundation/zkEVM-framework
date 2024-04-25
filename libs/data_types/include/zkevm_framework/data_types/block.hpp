/**
 * @file block.hpp
 * @brief This file defines Block.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HPP_

#include <expected>
#include <iostream>

#include "zkevm_framework/data_types/account_block.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/block_header.hpp"
#include "zkevm_framework/data_types/errors.hpp"
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
        std::expected<bytes, SerializationError> serialize() const noexcept;

        /// @brief write SSZ serialization to stream
        std::expected<void, SerializationError> serialize(std::ostream &out) const noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<Block, SerializationError> deserialize(const bytes &src) noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<Block, SerializationError> deserialize(std::istream &src) noexcept;
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HPP_
