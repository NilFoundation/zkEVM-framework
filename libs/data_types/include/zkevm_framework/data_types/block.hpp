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
        uint64_t m_blockNumber;
        SSZHash m_prevBlock;
        SSZHash m_smartContractsRoot;
        SSZHash m_inMessagesRoot;
        SSZHash m_outMessagesRoot;
        uint64_t m_outMessagesNum;
        SSZHash m_receiptsRoot;
        SSZHash m_childBlocksRootHash;
        SSZHash m_masterChainHash;
        Bloom m_logsBloom;
        uint64_t m_timestamp;

        Block(uint64_t blockNumber, const SSZHash& prevBlock, const SSZHash& smartContractsRoot,
              const SSZHash& inMessagesRoot, const SSZHash& outMessagesRoot,
              uint64_t outMessagesNum, const SSZHash& receiptsRoot,
              const SSZHash& childBlocksRootHash, const SSZHash& masterChainHash,
              const Bloom& logsBloom, uint64_t timestamp)
            : m_blockNumber(blockNumber),
              m_prevBlock(prevBlock),
              m_smartContractsRoot(smartContractsRoot),
              m_inMessagesRoot(inMessagesRoot),
              m_outMessagesRoot(outMessagesRoot),
              m_outMessagesNum(outMessagesNum),
              m_receiptsRoot(receiptsRoot),
              m_childBlocksRootHash(childBlocksRootHash),
              m_masterChainHash(masterChainHash),
              m_logsBloom(logsBloom),
              m_timestamp(timestamp) {}

        /// @brief Default constructor - zero initializer
        Block() {}

        /// @returns the SSZ serialization
        std::expected<bytes, SerializationError> serialize() const noexcept;

        /// @brief write SSZ serialization to stream
        std::expected<void, SerializationError> serialize(std::ostream& out) const noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<Block, SerializationError> deserialize(const bytes& src) noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<Block, SerializationError> deserialize(std::istream& src) noexcept;
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HPP_
