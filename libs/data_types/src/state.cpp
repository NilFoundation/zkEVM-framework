#include "zkevm_framework/data_types/state.hpp"

#include "containers/state.hpp"
#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    using namespace containers;

    bytes State::serialize() const { return ssz::serialize<StateContainer>(*this); }

    State State::deserialize(const bytes &src) {
        return static_cast<State>(ssz::deserialize<StateContainer>(src));
    }
}  // namespace data_types
