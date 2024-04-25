/**
 * @file block_header.hpp
 * @brief This file defines Block header.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HEADER_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HEADER_HPP_

#include <cstdint>
#include <expected>
#include <iostream>

#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/errors.hpp"

namespace data_types {
    /// @brief Block header.
    class BlockHeader {
      public:
        friend class Block;

        /// @brief Block data type.
        enum BlockDataType { HeaderData, BlockData };

        Hash m_parentHash;
        size_t m_number;
        size_t m_gasLimit;
        size_t m_gasUsed;
        Address m_coinbase;
        size_t m_prevrandao;
        size_t m_chain_id;
        size_t m_basefee;
        size_t m_blob_basefee;
        bytes m_extraData;
        size_t m_timestamp;

        static constexpr size_t Invalid256 = 0;  // TODO: define this

        /// @brief block header from data
        BlockHeader(Hash const& parentHash, size_t number, size_t gasLimit, size_t gasUsed,
                    Address coinbase, size_t prevrandao, size_t chain_id, size_t basefee,
                    size_t blob_basefee, const bytes& extraData, size_t timestamp = Invalid256)
            : m_parentHash(parentHash),
              m_number(number),
              m_gasLimit(gasLimit),
              m_gasUsed(gasUsed),
              m_coinbase(coinbase),
              m_prevrandao(prevrandao),
              m_chain_id(chain_id),
              m_basefee(basefee),
              m_blob_basefee(blob_basefee),
              m_extraData(extraData),
              m_timestamp(timestamp) {}

        /// @returns the SSZ serialization
        std::expected<bytes, SerializationError> serialize() const noexcept;

        /// @brief write SSZ serialization to stream
        std::expected<void, SerializationError> serialize(std::ostream& out) const noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<BlockHeader, SerializationError> deserialize(
            const bytes& src) noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<BlockHeader, SerializationError> deserialize(
            std::istream& src) noexcept;
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HEADER_HPP_
