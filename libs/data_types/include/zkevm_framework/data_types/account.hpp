/**
 * @file account.hpp
 * @brief This file defines Ethereum account.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ACCOUNT_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ACCOUNT_HPP_

#include <cstddef>
#include <cstdint>

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    /// @brief Ethereum account.
    class Account {
      public:
        friend class State;

        Account() {}

        Account(size_t nonce, size_t balance, AccessStatus access_status, Hash root)
            : m_nonce(nonce),
              m_balance(balance),
              m_accessStatus(access_status),
              m_storageRoot(root) {}

        size_t getNonce() const { return m_nonce; }
        size_t getBalance() const { return m_balance; }
        AccessStatus getAccessStatus() const { return m_accessStatus; }
        Hash getStorageRoot() const { return m_storageRoot; }

        /// @returns the SSZ serialization
        bytes serialize() const;

        /// @brief deserizalize from SSZ
        static Account deserialize(const bytes &src);

      private:
        size_t m_nonce;
        size_t m_balance;
        AccessStatus m_accessStatus;
        Hash m_storageRoot;

        /// @brief Serializable mirroring structure of `Account`.
        struct Serializable : ssz::ssz_container {
            size_t m_nonce;
            size_t m_balance;
            uint8_t m_accessStatus;
            Hash m_storageRoot;

            SSZ_CONT(m_nonce, m_balance, m_accessStatus, m_storageRoot)

            Serializable() {}

            Serializable(const Account &account)
                : m_nonce(account.m_nonce),
                  m_balance(account.m_balance),
                  m_storageRoot(account.m_storageRoot),
                  m_accessStatus(account.m_accessStatus) {}
        };

        Account(const Serializable &s)
            : m_balance(s.m_balance),
              m_nonce(s.m_nonce),
              m_storageRoot(s.m_storageRoot),
              m_accessStatus(static_cast<AccessStatus>(s.m_accessStatus)) {}
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_ACCOUNT_HPP_
