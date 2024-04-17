#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_BLOCK_HEADER_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_BLOCK_HEADER_HPP_

#include <cstddef>
#include <cstdint>

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/block_header.hpp"

namespace data_types {
    namespace containers {
        struct BlockHeaderContainer : ssz::ssz_variable_size_container {
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

            BlockHeaderContainer() {}

            BlockHeaderContainer(const BlockHeader& header)
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

            operator BlockHeader() const {
                return BlockHeader(m_parentHash, m_number, m_gasLimit, m_gasUsed, m_coinbase,
                                   m_prevrandao, m_chain_id, m_basefee, m_blob_basefee,
                                   m_extraData.data(), m_timestamp);
            }
        };
    }  // namespace containers
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_BLOCK_HEADER_HPP_
