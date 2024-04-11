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

        /// @brief deserizalize from SSZ
        static BlockHeader deserialize(const bytes& src);

      private:
        struct Serializable : ssz::ssz_variable_size_container {
            Hash m_parentHash;
            size_t m_number;
            size_t m_gasLimit;
            size_t m_gasUsed;
            Address m_coinbase;
            size_t m_prevrandao;
            size_t m_chain_id;
            size_t m_basefee;
            size_t m_blob_basefee;
            ssz::list<std::byte, 100> m_extraData;
            size_t m_timestamp;

            SSZ_CONT(m_parentHash, m_number, m_gasLimit, m_gasUsed, m_coinbase, m_prevrandao,
                     m_chain_id, m_basefee, m_blob_basefee, m_extraData, m_timestamp)

            Serializable() {}

            Serializable(const BlockHeader& header)
                : m_parentHash(header.m_parentHash),
                  m_number(header.m_number),
                  m_gasLimit(header.m_gasLimit),
                  m_gasUsed(header.m_gasUsed),
                  m_coinbase(header.m_coinbase),
                  m_prevrandao(header.m_prevrandao),
                  m_chain_id(header.m_chain_id),
                  m_basefee(header.m_basefee),
                  m_blob_basefee(header.m_basefee),
                  m_extraData(header.m_extraData),
                  m_timestamp(header.m_timestamp) {}
        };

        BlockHeader(const Serializable& s)
            : m_parentHash(s.m_parentHash),
              m_number(s.m_number),
              m_gasLimit(s.m_gasLimit),
              m_gasUsed(s.m_gasUsed),
              m_coinbase(s.m_coinbase),
              m_prevrandao(s.m_prevrandao),
              m_chain_id(s.m_chain_id),
              m_basefee(s.m_basefee),
              m_blob_basefee(s.m_basefee),
              m_extraData(s.m_extraData.begin(), s.m_extraData.end()),
              m_timestamp(s.m_timestamp) {}
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HEADER_HPP_
