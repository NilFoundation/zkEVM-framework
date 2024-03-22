#include "zkevm_framework/data_types/state.hpp"

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    State::Serializable::Serializable(const State &state) {
        for (auto [addr, acc] : state.m_accounts) {
            m_addresses.push_back(addr);
            m_accounts.push_back(acc);
        }
    }

    State::State(const State::Serializable &s) {
        for (size_t i = 0; i < s.m_accounts.size(); ++i) {
            m_accounts.insert({s.m_addresses[i], s.m_accounts[i]});
        }
    }

    bytes State::serialize() const { return ssz::serialize<State::Serializable>(*this); }

    State State::deserialize(const bytes &src) {
        return ssz::deserialize<State::Serializable>(src);
    }
}  // namespace data_types
