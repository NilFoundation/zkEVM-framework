#include "zkevm_framework/data_types/message.hpp"

#include "containers/message.hpp"
#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    using namespace containers;

    bytes CommonMsgInfo::serialize() const { return ssz::serialize<CommonMsgInfoContainer>(*this); }

    CommonMsgInfo CommonMsgInfo::deserialize(const bytes& src) {
        return static_cast<CommonMsgInfo>(ssz::deserialize<CommonMsgInfoContainer>(src));
    }

    bytes InMsg::serialize() const { return ssz::serialize<InMsgContainer>(*this); }

    InMsg InMsg::deserialize(const bytes& src) {
        return static_cast<InMsg>(ssz::deserialize<InMsgContainer>(src));
    }

    bytes OutMsg::serialize() const { return ssz::serialize<OutMsgContainer>(*this); }

    OutMsg OutMsg::deserialize(const bytes& src) {
        return static_cast<OutMsg>(ssz::deserialize<OutMsgContainer>(src));
    }
}  // namespace data_types
