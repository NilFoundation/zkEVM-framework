/**
 * @file block.hpp
 * @brief This file defines Block.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HPP_

#include <vector>

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/block_header.hpp"
#include "zkevm_framework/data_types/transaction.hpp"
#include "zkevm_framework/data_types/transaction_receipt.hpp"

namespace data_types {
    /// @brief Block.
    class Block {
      public:
        /// @brief empty block
        Block() {}

        Block(const std::vector<Transaction> &transactions,
              const std::vector<TransactionReceipt> &receipts, const BlockHeader &previousBlock,
              const BlockHeader &currentBlock)
            : m_transactions(transactions),
              m_receipts(receipts),
              m_previousBlock(previousBlock),
              m_currentBlock(currentBlock) {}

        /// @returns the SSZ serialization
        bytes serialize() const;

        /// @brief deserizalize from SSZ
        static Block deserialize(const bytes &src);

        std::vector<Transaction> const &getTransactions() const { return m_transactions; }
        std::vector<TransactionReceipt> const &getReceipts() const { return m_receipts; }
        BlockHeader const &getCurrentBlock() const { return m_currentBlock; }
        BlockHeader const &getPreviousBlock() const { return m_previousBlock; }

      private:
        std::vector<Transaction> m_transactions;
        std::vector<TransactionReceipt> m_receipts;
        BlockHeader m_previousBlock;
        BlockHeader m_currentBlock;

        /// @brief Serializable mirroring structure of `Block`.
        struct Serializable : ssz::ssz_variable_size_container {
            ssz::list<Transaction::Serializable, 100> m_transactions;
            ssz::list<TransactionReceipt::Serializable, 100> m_receipts;
            BlockHeader::Serializable m_previousBlock;
            BlockHeader::Serializable m_currentBlock;

            SSZ_CONT(m_transactions, m_receipts, m_previousBlock, m_currentBlock)

            Serializable() {}

            Serializable(const Block &block);
        };

        Block(const Serializable &s);
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_BLOCK_HPP_
