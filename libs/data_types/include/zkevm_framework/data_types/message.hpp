/**
 * @file message.hpp
 * @brief This file defines CommonMsgInfo, InMsg and OutMsg.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_MESSAGE_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_MESSAGE_HPP_

#include <cstring>
#include <expected>
#include <iostream>

#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/errors.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

namespace data_types {
    class Message {
      public:
        /// @brief Transaction type
        enum Type : uint8_t { ExecutionMessageKind, DeployMessageKind, RefundMessageKind };

        Type m_flags;
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
        bytes m_code;
        Signature m_signature;

        Message(Type flags, uint64_t chainId, uint64_t seqno, uint64_t gasPrice, uint64_t gasLimit,
                const Address& from, const Address& to, const Address& refundTo,
                const Address& bounceTo, uint64_t value, const CurrencyBalance& currency,
                bytes const& code, const Signature& signature)
            : m_flags(flags),
              m_chainId(chainId),
              m_seqno(seqno),
              m_gasPrice(gasPrice),
              m_gasLimit(gasLimit),
              m_from(from),
              m_to(to),
              m_refundTo(refundTo),
              m_bounceTo(bounceTo),
              m_value(value),
              m_currency(currency),
              m_code(code),
              m_signature(signature) {}

        /// @brief Default constructor - zero initializer
        Message() {}

        /// @returns the SSZ serialisation
        std::expected<bytes, SerializationError> serialize() const noexcept;

        /// @brief write SSZ serialization to stream
        std::expected<void, SerializationError> serialize(std::ostream& out) const noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<Message, SerializationError> deserialize(const bytes& src) noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<Message, SerializationError> deserialize(std::istream& src) noexcept;
    };

    class CommonMsgInfo {
      public:
        friend class InMsg;
        friend class OutMsg;

        Address m_src;
        Address m_dst;
        size_t m_value;

        CommonMsgInfo(Address src, Address dst, size_t value)
            : m_src(src), m_dst(dst), m_value(value) {}

        /// @brief Default constructor - zero initializer
        CommonMsgInfo() : m_value(0) { std::memset(m_src.data(), 0, m_src.size()); }

        /// @returns the SSZ serialisation
        std::expected<bytes, SerializationError> serialize() const noexcept;

        /// @brief write SSZ serialization to stream
        std::expected<void, SerializationError> serialize(std::ostream& out) const noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<CommonMsgInfo, SerializationError> deserialize(
            const bytes& src) noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<CommonMsgInfo, SerializationError> deserialize(
            std::istream& src) noexcept;
    };

    class InMsg {
      public:
        friend class Block;

        CommonMsgInfo m_info;
        Transaction m_transaction;

        InMsg(CommonMsgInfo info, Transaction transaction)
            : m_info(info), m_transaction(transaction) {}

        /// @brief Default constructor - zero initializer
        InMsg() {}

        /// @returns the SSZ serialisation
        std::expected<bytes, SerializationError> serialize() const noexcept;

        /// @brief write SSZ serialization to stream
        std::expected<void, SerializationError> serialize(std::ostream& out) const noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<InMsg, SerializationError> deserialize(const bytes& src) noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<InMsg, SerializationError> deserialize(std::istream& src) noexcept;
    };

    class OutMsg {
      public:
        friend class Block;
        friend class State;

        CommonMsgInfo m_info;
        Transaction m_transaction;

        OutMsg(CommonMsgInfo info, Transaction transaction)
            : m_info(info), m_transaction(transaction) {}

        /// @brief Default constructor - zero initializer
        OutMsg() {}

        /// @returns the SSZ serialisation
        std::expected<bytes, SerializationError> serialize() const noexcept;

        /// @brief write SSZ serialization to stream
        std::expected<void, SerializationError> serialize(std::ostream& out) const noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<OutMsg, SerializationError> deserialize(const bytes& src) noexcept;

        /// @brief deserizalize from SSZ
        static std::expected<OutMsg, SerializationError> deserialize(std::istream& src) noexcept;
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_MESSAGE_HPP_
