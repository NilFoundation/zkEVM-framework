/**
 * @file block_header.hpp
 * @brief This file defines Block header.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HEADER_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HEADER_HPP_

#include <cstddef>
#include <cstdint>

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

namespace data_types {
    /// @brief Block header.
    class BlockHeader {
      public:
        friend class Block;

        const static size_t Invalid256 = 0;  // TODO: define this

        /// @brief Block data type.
        enum BlockDataType { HeaderData, BlockData };

        /// @brief empty block header
        BlockHeader() {}

        /// @brief block header from data
        BlockHeader(Hash const& parentHash, Hash const& stateRoot, Hash const& transactionsRoot,
                    Hash const& receiptsRoot, size_t number, size_t gasLimit, size_t gasUsed,
                    const bytes& extraData, Address const& author, size_t timestamp = Invalid256)
            : m_parentHash(parentHash),
              m_stateRoot(stateRoot),
              m_transactionsRoot(transactionsRoot),
              m_receiptsRoot(receiptsRoot),
              m_number(number),
              m_gasLimit(gasLimit),
              m_gasUsed(gasUsed),
              m_extraData(extraData),
              m_timestamp(timestamp),
              m_author(author) {}

        Hash parentHash() const { return m_parentHash; }
        size_t timestamp() const { return m_timestamp; }
        Address const& author() const { return m_author; }
        Hash stateRoot() const { return m_stateRoot; }
        Hash transactionsRoot() const { return m_transactionsRoot; }
        Hash receiptsRoot() const { return m_receiptsRoot; }
        size_t gasUsed() const { return m_gasUsed; }
        size_t number() const { return m_number; }
        size_t gasLimit() const { return m_gasLimit; }
        bytes extraData() const { return m_extraData; }

        /// @returns the SSZ serialization
        bytes serialize() const;

        /// @brief deserizalize from SSZ
        static BlockHeader deserialize(const bytes& src);

      private:
        Hash m_parentHash;
        Hash m_stateRoot;
        Hash m_transactionsRoot;
        Hash m_receiptsRoot;
        size_t m_number;
        size_t m_gasLimit;
        size_t m_gasUsed;
        bytes m_extraData;
        size_t m_timestamp;
        Address m_author;

        /// @brief Serializable mirroring structure of `BlockHeader`.
        struct Serializable : ssz::ssz_variable_size_container {
            Hash m_parentHash;
            Hash m_stateRoot;
            Hash m_transactionsRoot;
            Hash m_receiptsRoot;
            size_t m_number;
            size_t m_gasLimit;
            size_t m_gasUsed;
            ssz::list<std::byte, 100> m_extraData;
            size_t m_timestamp;
            Address m_author;

            SSZ_CONT(m_parentHash, m_stateRoot, m_transactionsRoot, m_receiptsRoot, m_number,
                     m_gasLimit, m_gasUsed, m_extraData, m_timestamp, m_author)

            Serializable() {}

            Serializable(const BlockHeader& header)
                : m_parentHash(header.m_parentHash),
                  m_stateRoot(header.m_stateRoot),
                  m_transactionsRoot(header.m_transactionsRoot),
                  m_receiptsRoot(header.m_receiptsRoot),
                  m_number(header.m_number),
                  m_gasLimit(header.m_gasLimit),
                  m_gasUsed(header.m_gasUsed),
                  m_timestamp(header.m_timestamp),
                  m_author(header.m_author),
                  m_extraData(header.m_extraData) {}
        };

        BlockHeader(const Serializable& s)
            : m_parentHash(s.m_parentHash),
              m_stateRoot(s.m_stateRoot),
              m_transactionsRoot(s.m_transactionsRoot),
              m_receiptsRoot(s.m_receiptsRoot),
              m_number(s.m_number),
              m_gasLimit(s.m_gasLimit),
              m_gasUsed(s.m_gasUsed),
              m_timestamp(s.m_timestamp),
              m_author(s.m_author),
              m_extraData(s.m_extraData.begin(), s.m_extraData.end()) {}
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HEADER_HPP_
