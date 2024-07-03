#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_BLOCK_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_BLOCK_HPP_

#include "containers/account_block.hpp"
#include "containers/block_header.hpp"
#include "containers/message.hpp"
#include "ssz++.hpp"
#include "zkevm_framework/data_types/account_block.hpp"
#include "zkevm_framework/data_types/block.hpp"
#include "zkevm_framework/data_types/message.hpp"
#include "zkevm_framework/data_types/mpt.hpp"

namespace data_types {
    namespace containers {
        struct BlockContainer : ssz::ssz_variable_size_container {
            ssz::list<AccountBlockContainer, 100> m_accountBlocks;
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

            SSZ_CONT(m_blockNumber, m_prevBlock, m_smartContractsRoot, m_inMessagesRoot,
                     m_outMessagesRoot, m_outMessagesNum, m_receiptsRoot, m_childBlocksRootHash,
                     m_masterChainHash, m_logsBloom, m_timestamp)

            BlockContainer() {}

            BlockContainer(const Block& block)
                : m_blockNumber(block.m_blockNumber),
                  m_prevBlock(block.m_prevBlock),
                  m_smartContractsRoot(block.m_smartContractsRoot),
                  m_inMessagesRoot(block.m_inMessagesRoot),
                  m_outMessagesRoot(block.m_outMessagesRoot),
                  m_outMessagesNum(block.m_outMessagesNum),
                  m_receiptsRoot(block.m_receiptsRoot),
                  m_childBlocksRootHash(block.m_childBlocksRootHash),
                  m_masterChainHash(block.m_masterChainHash),
                  m_logsBloom(block.m_logsBloom),
                  m_timestamp(block.m_timestamp) {}

            operator Block() const {
                return Block(m_blockNumber, m_prevBlock, m_smartContractsRoot, m_inMessagesRoot,
                             m_outMessagesRoot, m_outMessagesNum, m_receiptsRoot,
                             m_childBlocksRootHash, m_masterChainHash, m_logsBloom, m_timestamp);
            }
        };
    }  // namespace containers
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_BLOCK_HPP_
