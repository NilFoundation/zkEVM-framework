#include "zkevm_framework/data_types/state.hpp"

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/account.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/message.hpp"

namespace data_types {
    State::Serializable::Serializable(const State &state)
        : m_globalId(state.m_globalId),
          m_shardId(state.m_shardId),
          m_seqNo(state.m_seqNo),
          m_vertSeqNo(state.m_vertSeqNo),
          m_genLt(state.m_genLt),
          m_minRefMcSeqno(state.m_minRefMcSeqno),
          m_beforeSplit(state.m_beforeSplit),
          m_genUtime(state.m_genUtime),
          m_totalBalance(state.m_totalBalance),
          m_totalValidatorFees(state.m_totalValidatorFees) {
        for (const OutMsg &out_msg : state.m_outMsg) {
            m_outMsg.push_back(OutMsg::Serializable(out_msg));
        }
        for (const Account &account : state.m_accounts) {
            m_accounts.push_back(Account::Serializable(account));
        }
    }

    State::State(const State::Serializable &s)
        : m_globalId(s.m_globalId),
          m_shardId(s.m_shardId),
          m_seqNo(s.m_seqNo),
          m_vertSeqNo(s.m_vertSeqNo),
          m_genLt(s.m_genLt),
          m_minRefMcSeqno(s.m_minRefMcSeqno),
          m_beforeSplit(s.m_beforeSplit),
          m_genUtime(s.m_genUtime),
          m_totalBalance(s.m_totalBalance),
          m_totalValidatorFees(s.m_totalValidatorFees) {
        for (const OutMsg::Serializable &out_msg : s.m_outMsg) {
            m_outMsg.push_back(OutMsg(out_msg));
        }
        for (const Account::Serializable &account : s.m_accounts) {
            m_accounts.push_back(Account(account));
        }
    }

    bytes State::serialize() const { return ssz::serialize<State::Serializable>(*this); }

    State State::deserialize(const bytes &src) {
        return ssz::deserialize<State::Serializable>(src);
    }
}  // namespace data_types
