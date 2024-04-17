/**
 * @file account.hpp
 * @brief This file defines Account.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ACCOUNT_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ACCOUNT_HPP_

#include <cstdint>

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
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ACCOUNT_HPP_
