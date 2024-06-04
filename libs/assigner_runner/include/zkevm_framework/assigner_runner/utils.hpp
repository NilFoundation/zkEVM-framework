#ifndef ZKEMV_FRAMEWORK_LIBS_ASSIGNER_RUNNER_INCLUDE_ZKEVM_FRAMEWORK_ASSIGNER_RUNNER_UTILS_HPP_
#define ZKEMV_FRAMEWORK_LIBS_ASSIGNER_RUNNER_INCLUDE_ZKEVM_FRAMEWORK_ASSIGNER_RUNNER_UTILS_HPP_

#include <evmc/evmc.h>

#include <execution_state.hpp>
#include <instructions_opcodes.hpp>

#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

template<typename T>
evmc::uint256be to_uint256be(T v) {
    evmc::uint256be res;
    uint8_t mask = (1 << 8) - 1;
    for (uint8_t i = 0; i <= 31; i++) {
        res.bytes[i] = (v >> (i - 31) * 8);
    }
    return res;
}

evmc_call_kind evmc_msg_kind(const data_types::Transaction::Type& type);
evmc_address to_evmc_address(const data_types::Address& v);

std::string to_str(const evmc_address& v);
std::string to_str(const data_types::Address& v);
std::string to_str(const evmc_bytes32& v);
std::string to_str(const std::vector<uint8_t>& code);
std::string to_str(const data_types::Transaction::Type& type);
std::string to_str(evmc_status_code code);

#endif  // ZKEMV_FRAMEWORK_LIBS_ASSIGNER_RUNNER_INCLUDE_ZKEVM_FRAMEWORK_ASSIGNER_RUNNER_UTILS_HPP_
