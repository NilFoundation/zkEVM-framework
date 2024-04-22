#include "zkevm_framework/data_types/state.hpp"

#include <iostream>
#include <optional>

#include "containers/state.hpp"
#include "sszpp/ssz++.hpp"
#include "utils.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    using namespace containers;

    bytes State::serialize() const { return ssz::serialize<StateContainer>(*this); }

    int State::serialize(std::ostream& out) const { return write_bytes(serialize(), out); }

    State State::deserialize(const bytes& src) {
        return static_cast<State>(ssz::deserialize<StateContainer>(src));
    }

    std::optional<State> State::deserialize(std::istream& src) {
        return deserialize_from_stream<State>(src);
    }
}  // namespace data_types
