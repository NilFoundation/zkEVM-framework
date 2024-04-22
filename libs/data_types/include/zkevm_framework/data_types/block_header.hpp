/**
 * @file block_header.hpp
 * @brief This file defines Block header.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HEADER_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HEADER_HPP_

#include <cstdint>
#include <iostream>
#include <optional>

#include "zkevm_framework/data_types/base.hpp"

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
        bytes serialize() const;

        /**
         * @brief write SSZ serialization to stream
         *
         * @return number of bytes written. If I/O error occured, returns -1.
         */
        int serialize(std::ostream& out) const;

        /// @brief deserizalize from SSZ
        static BlockHeader deserialize(const bytes& src);

        /// @brief deserizalize from SSZ
        static std::optional<BlockHeader> deserialize(std::istream& src);
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HEADER_HPP_
