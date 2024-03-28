/**
 * @file state.hpp
 * @brief This file defines State.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_STATE_HPP_
#define ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_STATE_HPP_

#include <unordered_map>

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/account.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    /// @brief State.
    class State {
      public:
        State() {}
        State(std::unordered_map<Address, Account, AddressHasher> accounts)
            : m_accounts(accounts) {}

        std::unordered_map<Address, Account, AddressHasher> const &getAccounts() const {
            return m_accounts;
        }

        /// @returns the SSZ serialization
        bytes serialize() const;

        /// @brief deserizalize from SSZ
        static State deserialize(const bytes &src);

      private:
        std::unordered_map<Address, Account, AddressHasher> m_accounts;

        /// @brief Serializable mirroring structure of `State`.
        struct Serializable : ssz::ssz_variable_size_container {
            ssz::list<Address, 100> m_addresses;
            ssz::list<Account::Serializable, 100> m_accounts;

            SSZ_CONT(m_addresses, m_accounts)

            Serializable() {}

            Serializable(const State &state);
        };

        State(const Serializable &s);
    };
}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_DATA_TYPES_INCLUDE_ZKEVM_FRAMEWORK_DATA_TYPES_STATE_HPP_
