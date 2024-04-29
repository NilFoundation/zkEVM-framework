#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_MESSAGE_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_MESSAGE_HPP_

#include <cstdint>

#include "containers/transaction.hpp"
#include "ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/message.hpp"

namespace data_types {
    namespace containers {
        struct CommonMsgInfoContainer : ssz::ssz_container {
            Address m_src;
            Address m_dst;
            size_t m_value;

            SSZ_CONT(m_src, m_dst, m_value)

            CommonMsgInfoContainer() {}

            CommonMsgInfoContainer(const CommonMsgInfo& info)
                : m_src(info.m_src), m_dst(info.m_dst), m_value(info.m_value) {}

            operator CommonMsgInfo() const { return CommonMsgInfo(m_src, m_dst, m_value); }
        };

        struct InMsgContainer : ssz::ssz_variable_size_container {
            CommonMsgInfoContainer m_info;
            TransactionContainer m_transaction;

            SSZ_CONT(m_info, m_transaction)

            InMsgContainer() {}

            InMsgContainer(const InMsg& msg)
                : m_info(msg.m_info), m_transaction(msg.m_transaction) {}

            operator InMsg() const { return InMsg(m_info, m_transaction); }
        };

        struct OutMsgContainer : ssz::ssz_variable_size_container {
            CommonMsgInfoContainer m_info;
            TransactionContainer m_transaction;

            SSZ_CONT(m_info, m_transaction)

            OutMsgContainer() {}

            OutMsgContainer(const OutMsg& msg)
                : m_info(msg.m_info), m_transaction(msg.m_transaction) {}

            operator OutMsg() const { return OutMsg(m_info, m_transaction); }
        };
    }  // namespace containers
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_MESSAGE_HPP_
