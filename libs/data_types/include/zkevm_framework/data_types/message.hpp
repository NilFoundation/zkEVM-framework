/**
 * @file message.hpp
 * @brief This file defines CommonMsgInfo, InMsg and OutMsg.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_MESSAGE_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_MESSAGE_HPP_

#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

namespace data_types {
    class CommonMsgInfo {
      public:
        friend class InMsg;
        friend class OutMsg;

        Address m_src;
        Address m_dst;
        size_t m_value;

        CommonMsgInfo(Address src, Address dst, size_t value)
            : m_src(src), m_dst(dst), m_value(value) {}

        /// @returns the SSZ serialisation
        bytes serialize() const;

        /// @brief deserizalize from SSZ
        static CommonMsgInfo deserialize(const bytes& src);
    };

    class InMsg {
      public:
        friend class Block;

        CommonMsgInfo m_info;
        Transaction m_transaction;

        InMsg(CommonMsgInfo info, Transaction transaction)
            : m_info(info), m_transaction(transaction) {}

        /// @returns the SSZ serialisation
        bytes serialize() const;

        /// @brief deserizalize from SSZ
        static InMsg deserialize(const bytes& src);
    };

    class OutMsg {
      public:
        friend class Block;
        friend class State;

        CommonMsgInfo m_info;
        Transaction m_transaction;

        OutMsg(CommonMsgInfo info, Transaction transaction)
            : m_info(info), m_transaction(transaction) {}

        /// @returns the SSZ serialisation
        bytes serialize() const;

        /// @brief deserizalize from SSZ
        static OutMsg deserialize(const bytes& src);
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_MESSAGE_HPP_
