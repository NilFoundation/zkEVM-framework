#include "zkevm_framework/data_types/message.hpp"

#include <iostream>
#include <optional>

#include "containers/message.hpp"
#include "sszpp/ssz++.hpp"
#include "utils.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    using namespace containers;

    bytes CommonMsgInfo::serialize() const { return ssz::serialize<CommonMsgInfoContainer>(*this); }

    int CommonMsgInfo::serialize(std::ostream& out) const { return write_bytes(serialize(), out); }

    CommonMsgInfo CommonMsgInfo::deserialize(const bytes& src) {
        return static_cast<CommonMsgInfo>(ssz::deserialize<CommonMsgInfoContainer>(src));
    }

    std::optional<CommonMsgInfo> CommonMsgInfo::deserialize(std::istream& src) {
        return deserialize_from_stream<CommonMsgInfo>(src);
    }

    bytes InMsg::serialize() const { return ssz::serialize<InMsgContainer>(*this); }

    int InMsg::serialize(std::ostream& out) const { return write_bytes(serialize(), out); }

    InMsg InMsg::deserialize(const bytes& src) {
        return static_cast<InMsg>(ssz::deserialize<InMsgContainer>(src));
    }

    std::optional<InMsg> InMsg::deserialize(std::istream& src) {
        return deserialize_from_stream<InMsg>(src);
    }

    bytes OutMsg::serialize() const { return ssz::serialize<OutMsgContainer>(*this); }

    int OutMsg::serialize(std::ostream& out) const { return write_bytes(serialize(), out); }

    OutMsg OutMsg::deserialize(const bytes& src) {
        return static_cast<OutMsg>(ssz::deserialize<OutMsgContainer>(src));
    }

    std::optional<OutMsg> OutMsg::deserialize(std::istream& src) {
        return deserialize_from_stream<OutMsg>(src);
    }
}  // namespace data_types
