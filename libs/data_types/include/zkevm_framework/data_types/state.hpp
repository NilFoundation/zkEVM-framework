/**
 * @file state.hpp
 * @brief This file defines State.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_STATE_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_STATE_HPP_

#include <cstdint>

#include "zkevm_framework/data_types/account.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/message.hpp"
#include "zkevm_framework/data_types/mpt.hpp"

namespace data_types {
    /// @brief State.
    class State {
      public:
        int m_globalId;
        int m_shardId;
        uint32_t m_seqNo;
        int m_vertSeqNo;
        size_t m_genLt;
        uint32_t m_minRefMcSeqno;
        bool m_beforeSplit;
        uint32_t m_genUtime;
        uint32_t m_totalBalance;
        uint32_t m_totalValidatorFees;
        MPTNode<OutMsg> m_outMsg;
        MPTNode<Account> m_accounts;

        State(int globalId, int shardId, uint32_t seqNo, int vertSeqNo, size_t genLt,
              uint32_t minRefMcSeqno, bool beforeSplit, uint32_t genUtime, uint32_t totalBalance,
              uint32_t totalValidatorFees, const MPTNode<OutMsg> &outMsg,
              const MPTNode<Account> &accounts)
            : m_globalId(globalId),
              m_shardId(shardId),
              m_seqNo(seqNo),
              m_vertSeqNo(vertSeqNo),
              m_genLt(genLt),
              m_minRefMcSeqno(minRefMcSeqno),
              m_beforeSplit(beforeSplit),
              m_genUtime(genUtime),
              m_totalBalance(totalBalance),
              m_totalValidatorFees(totalValidatorFees),
              m_outMsg(outMsg),
              m_accounts(accounts) {}

        /// @returns the SSZ serialization
        bytes serialize() const;

        /// @brief deserizalize from SSZ
        static State deserialize(const bytes &src);
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_STATE_HPP_
