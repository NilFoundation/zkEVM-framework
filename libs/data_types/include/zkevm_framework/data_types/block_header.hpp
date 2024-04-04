/**
 * @file block_header.hpp
 * @brief This file defines Block header.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HEADER_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HEADER_HPP_

#include <cstdint>

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

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
        bytes m_extraData;
        size_t m_timestamp;

        static constexpr size_t Invalid256 = 0;  // TODO: define this

        /// @brief block header from data
        BlockHeader(Hash const& parentHash, size_t number, size_t gasLimit, size_t gasUsed,
                    const bytes& extraData, size_t timestamp = Invalid256)
            : m_parentHash(parentHash),
              m_number(number),
              m_gasLimit(gasLimit),
              m_gasUsed(gasUsed),
              m_extraData(extraData),
              m_timestamp(timestamp) {}

        /// @returns the SSZ serialization
        bytes serialize() const;

        /// @brief deserizalize from SSZ
        static BlockHeader deserialize(const bytes& src);

      private:
        struct Serializable : ssz::ssz_variable_size_container {
            Hash m_parentHash;
            size_t m_number;
            size_t m_gasLimit;
            size_t m_gasUsed;
            ssz::list<std::byte, 100> m_extraData;
            size_t m_timestamp;

            SSZ_CONT(m_parentHash, m_number, m_gasLimit, m_gasUsed, m_extraData, m_timestamp)

            Serializable() {}

            Serializable(const BlockHeader& header)
                : m_parentHash(header.m_parentHash),
                  m_number(header.m_number),
                  m_gasLimit(header.m_gasLimit),
                  m_gasUsed(header.m_gasUsed),
                  m_timestamp(header.m_timestamp),
                  m_extraData(header.m_extraData) {}
        };

        BlockHeader(const Serializable& s)
            : m_parentHash(s.m_parentHash),
              m_number(s.m_number),
              m_gasLimit(s.m_gasLimit),
              m_gasUsed(s.m_gasUsed),
              m_timestamp(s.m_timestamp),
              m_extraData(s.m_extraData.begin(), s.m_extraData.end()) {}
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HEADER_HPP_
