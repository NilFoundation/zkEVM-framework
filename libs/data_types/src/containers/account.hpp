#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_ACCOUNT_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_ACCOUNT_HPP_

#include <cstdint>

#include "ssz++.hpp"
#include "zkevm_framework/data_types/account.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    namespace containers {
        struct AccountContainer : ssz::ssz_container {
            size_t m_nonce;
            size_t m_balance;
            uint8_t m_accessStatus;

            SSZ_CONT(m_nonce, m_balance, m_accessStatus)

            AccountContainer() {}

            AccountContainer(const Account &account)
                : m_nonce(account.m_nonce),
                  m_balance(account.m_balance),
                  m_accessStatus(account.m_accessStatus) {}

            operator Account() const {
                return Account(m_nonce, m_balance, static_cast<AccessStatus>(m_accessStatus));
            }
        };
    }  // namespace containers
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_SRC_CONTAINERS_ACCOUNT_HPP_
