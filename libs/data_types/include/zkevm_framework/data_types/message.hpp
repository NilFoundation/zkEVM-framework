/**
 * @file message.hpp
 * @brief This file defines CommonMsgInfo, InMsg and OutMsg.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_MESSAGE_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_MESSAGE_HPP_

#include "sszpp/ssz++.hpp"
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

      private:
        struct Serializable : ssz::ssz_container {
            Address m_src;
            Address m_dst;
            size_t m_value;

            SSZ_CONT(m_src, m_dst, m_value)

            Serializable() {}

            Serializable(const CommonMsgInfo& info)
                : m_src(info.m_src), m_dst(info.m_dst), m_value(info.m_value) {}
        };

        CommonMsgInfo(const Serializable& s) : m_src(s.m_src), m_dst(s.m_dst), m_value(s.m_value) {}
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

      private:
        struct Serializable : ssz::ssz_variable_size_container {
            CommonMsgInfo::Serializable m_info;
            Transaction::Serializable m_transaction;

            SSZ_CONT(m_info, m_transaction)

            Serializable() {}

            Serializable(const InMsg& msg) : m_info(msg.m_info), m_transaction(msg.m_transaction) {}
        };

        InMsg(const Serializable& s) : m_info(s.m_info), m_transaction(s.m_transaction) {}
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

      private:
        struct Serializable : ssz::ssz_variable_size_container {
            CommonMsgInfo::Serializable m_info;
            Transaction::Serializable m_transaction;

            SSZ_CONT(m_info, m_transaction)

            Serializable() {}

            Serializable(const OutMsg& msg)
                : m_info(msg.m_info), m_transaction(msg.m_transaction) {}
        };

        OutMsg(const Serializable& s) : m_info(s.m_info), m_transaction(s.m_transaction) {}
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_MESSAGE_HPP_
