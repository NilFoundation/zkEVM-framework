#include "zkevm_framework/data_types/message.hpp"

#include "containers/message.hpp"
#include "utils.hpp"

namespace data_types {
    DEFAULT_IMPL_SERDE_METHODS(Message, containers::MessageContainer)
    DEFAULT_IMPL_SERDE_METHODS(CommonMsgInfo, containers::CommonMsgInfoContainer)
    DEFAULT_IMPL_SERDE_METHODS(InMsg, containers::InMsgContainer)
    DEFAULT_IMPL_SERDE_METHODS(OutMsg, containers::OutMsgContainer)
}  // namespace data_types
