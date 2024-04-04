/**
 * @file account.hpp
 * @brief This file defines Account.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ACCOUNT_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ACCOUNT_HPP_

#include <cstdint>

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    /// @brief Account.
    class Account {
      public:
        friend class State;

        size_t m_nonce;
        size_t m_balance;
        AccessStatus m_accessStatus;

        Account(size_t nonce, size_t balance, AccessStatus access_status)
            : m_nonce(nonce), m_balance(balance), m_accessStatus(access_status) {}

        /// @returns the SSZ serialization
        bytes serialize() const;

        /// @brief deserizalize from SSZ
        static Account deserialize(const bytes &src);

      private:
        struct Serializable : ssz::ssz_container {
            size_t m_nonce;
            size_t m_balance;
            uint8_t m_accessStatus;

            SSZ_CONT(m_nonce, m_balance, m_accessStatus)

            Serializable() {}

            Serializable(const Account &account)
                : m_nonce(account.m_nonce),
                  m_balance(account.m_balance),
                  m_accessStatus(account.m_accessStatus) {}
        };

        Account(const Serializable &s)
            : m_balance(s.m_balance),
              m_nonce(s.m_nonce),
              m_accessStatus(static_cast<AccessStatus>(s.m_accessStatus)) {}
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ACCOUNT_HPP_
