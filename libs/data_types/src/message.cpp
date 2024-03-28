#include "zkevm_framework/data_types/message.hpp"

#include "sszpp/ssz++.hpp"
#include "zkevm_framework/data_types/base.hpp"

namespace data_types {
    bytes CommonMsgInfo::serialize() const {
        return ssz::serialize<CommonMsgInfo::Serializable>(*this);
    }

    CommonMsgInfo CommonMsgInfo::deserialize(const bytes& src) {
        return ssz::deserialize<CommonMsgInfo::Serializable>(src);
    }

    bytes InMsg::serialize() const { return ssz::serialize<InMsg::Serializable>(*this); }

    InMsg InMsg::deserialize(const bytes& src) {
        return ssz::deserialize<InMsg::Serializable>(src);
    }

    bytes OutMsg::serialize() const { return ssz::serialize<OutMsg::Serializable>(*this); }

    OutMsg OutMsg::deserialize(const bytes& src) {
        return ssz::deserialize<OutMsg::Serializable>(src);
    }
}  // namespace data_types
