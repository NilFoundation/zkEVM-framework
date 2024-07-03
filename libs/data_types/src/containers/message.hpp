#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_MESSAGE_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_MESSAGE_HPP_

#include <cstdint>

#include "containers/transaction.hpp"
#include "ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/message.hpp"

namespace data_types {
    namespace containers {
        struct MessageContainer : ssz::ssz_container {
            uint8_t m_flags;
            uint64_t m_chainId;
            uint64_t m_seqno;
            uint64_t m_gasPrice;
            uint64_t m_gasLimit;
            Address m_from;
            Address m_to;
            Address m_refundTo;
            Address m_bounceTo;
            uint64_t m_value;
            CurrencyBalance m_currency;
            ssz::list<std::byte, 3072> m_code;
            Signature m_signature;

            SSZ_CONT(m_flags, m_chainId, m_seqno, m_gasPrice, m_gasLimit, m_from, m_to, m_refundTo,
                     m_bounceTo, m_value, m_currency, m_code, m_signature)

            MessageContainer() {}

            MessageContainer(const Message& msg)
                : m_flags(msg.m_flags),
                  m_chainId(msg.m_chainId),
                  m_seqno(msg.m_seqno),
                  m_gasPrice(msg.m_gasPrice),
                  m_gasLimit(msg.m_gasLimit),
                  m_from(msg.m_from),
                  m_to(msg.m_to),
                  m_refundTo(msg.m_refundTo),
                  m_bounceTo(msg.m_bounceTo),
                  m_value(msg.m_value),
                  m_currency(msg.m_currency),
                  m_code(msg.m_code),
                  m_signature(msg.m_signature) {}

            operator Message() const {
                return Message(static_cast<Message::Type>(m_flags), m_chainId, m_seqno, m_gasPrice,
                               m_gasLimit, m_from, m_to, m_refundTo, m_bounceTo, m_value,
                               m_currency, m_code.data(), m_signature);
            }
        };

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
