#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_STATE_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_STATE_HPP_

#include <cstdint>

#include "containers/account.hpp"
#include "containers/message.hpp"
#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/account.hpp"
#include "zkevm_framework/data_types/message.hpp"
#include "zkevm_framework/data_types/mpt.hpp"
#include "zkevm_framework/data_types/state.hpp"

namespace data_types {
    namespace containers {
        struct StateContainer : ssz::ssz_variable_size_container {
            uint64_t m_globalId;
            uint64_t m_shardId;
            uint32_t m_seqNo;
            uint64_t m_vertSeqNo;
            size_t m_genLt;
            uint32_t m_minRefMcSeqno;
            bool m_beforeSplit;
            uint32_t m_genUtime;
            uint32_t m_totalBalance;
            uint32_t m_totalValidatorFees;
            ssz::list<OutMsgContainer, 100> m_outMsg;
            ssz::list<AccountContainer, 100> m_accounts;

            SSZ_CONT(m_globalId, m_shardId, m_seqNo, m_vertSeqNo, m_genLt, m_minRefMcSeqno,
                     m_beforeSplit, m_genUtime, m_totalBalance, m_totalValidatorFees, m_outMsg,
                     m_accounts)

            StateContainer() {}

            StateContainer(const State &state)
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
                    m_outMsg.push_back(OutMsgContainer(out_msg));
                }
                for (const Account &account : state.m_accounts) {
                    m_accounts.push_back(AccountContainer(account));
                }
            }

            operator State() const {
                MPTNode<OutMsg> outMsg;
                for (const OutMsgContainer &msg : m_outMsg) {
                    outMsg.push_back(static_cast<OutMsg>(msg));
                }
                MPTNode<Account> accounts;
                for (const AccountContainer &account : m_accounts) {
                    accounts.push_back(static_cast<Account>(account));
                }
                return State(m_globalId, m_shardId, m_seqNo, m_vertSeqNo, m_genLt, m_minRefMcSeqno,
                             m_beforeSplit, m_genUtime, m_totalBalance, m_totalValidatorFees,
                             outMsg, accounts);
            }
        };
    }  // namespace containers
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_STATE_HPP_
