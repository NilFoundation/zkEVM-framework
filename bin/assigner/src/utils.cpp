#include "utils.h"

#include <evmc/evmc.h>

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

evmc_call_kind evmc_msg_kind(const data_types::Transaction::Type& type) {
    switch (type) {
        case data_types::Transaction::Type::ContractCreation:
            return EVMC_CREATE;
        case data_types::Transaction::Type::MessageCall:
            return EVMC_CALL;
        default:
            return EVMC_CALL;
    };
}

evmc_address to_evmc_address(const data_types::Address& v) {
    evmc_address res = {
        v[0],  v[1],  v[2],  v[3],  v[4],  v[5],  v[6],  v[7],  v[8],  v[9],
        v[10], v[11], v[12], v[13], v[14], v[15], v[16], v[17], v[18], v[19],
    };
    return res;
}

static std::string byte_array_to_str(const uint8_t* data, size_t size) {
    std::stringstream os;
    os << std::hex << "0x";
    for (int i = 0; i < size; ++i) {
        os << std::setw(2) << std::setfill('0') << (int)data[i];
    }
    return os.str();
}

std::string to_str(const evmc_address& v) { return byte_array_to_str(v.bytes, sizeof(v.bytes)); }

std::string to_str(const data_types::Address& v) { return byte_array_to_str(v.data(), v.size()); }

std::string to_str(const evmc_bytes32& v) { return byte_array_to_str(v.bytes, sizeof(v.bytes)); }

std::string to_str(const std::vector<uint8_t>& code) {
    return byte_array_to_str(code.data(), code.size());
}

std::string to_str(const data_types::Transaction::Type& type) {
    switch (type) {
        case data_types::Transaction::Type::NullTransaction:
            return "NullTransaction";
        case data_types::Transaction::Type::ContractCreation:
            return "ContractCreation";
        case data_types::Transaction::Type::MessageCall:
            return "MessageCall";
        default:
            return "none";
    };
}

std::string to_str(evmc_status_code code) {
    switch (code) {
        case EVMC_SUCCESS:
            return "EVMC_SUCCESS";
        case EVMC_FAILURE:
            return "EVMC_FAILURE";
        case EVMC_REVERT:
            return "EVMC_REVERT";
        case EVMC_OUT_OF_GAS:
            return "EVMC_OUT_OF_GAS";
        case EVMC_INVALID_INSTRUCTION:
            return "EVMC_INVALID_INSTRUCTION";
        case EVMC_UNDEFINED_INSTRUCTION:
            return "EVMC_UNDEFINED_INSTRUCTION";
        case EVMC_STACK_OVERFLOW:
            return "EVMC_STACK_OVERFLOW";
        case EVMC_STACK_UNDERFLOW:
            return "EVMC_STACK_UNDERFLOW";
        case EVMC_BAD_JUMP_DESTINATION:
            return "EVMC_BAD_JUMP_DESTINATION";
        case EVMC_INVALID_MEMORY_ACCESS:
            return "EVMC_INVALID_MEMORY_ACCESS";
        case EVMC_CALL_DEPTH_EXCEEDED:
            return "EVMC_CALL_DEPTH_EXCEEDED";
        case EVMC_STATIC_MODE_VIOLATION:
            return "EVMC_STATIC_MODE_VIOLATION";
        case EVMC_PRECOMPILE_FAILURE:
            return "EVMC_PRECOMPILE_FAILURE";
        case EVMC_CONTRACT_VALIDATION_FAILURE:
            return "EVMC_CONTRACT_VALIDATION_FAILURE";
        case EVMC_ARGUMENT_OUT_OF_RANGE:
            return "EVMC_ARGUMENT_OUT_OF_RANGE";
        case EVMC_WASM_UNREACHABLE_INSTRUCTION:
            return "EVMC_WASM_UNREACHABLE_INSTRUCTION";
        case EVMC_WASM_TRAP:
            return "EVMC_WASM_TRAP";
        case EVMC_INSUFFICIENT_BALANCE:
            return "EVMC_INSUFFICIENT_BALANCE";
        case EVMC_INTERNAL_ERROR:
            return "EVMC_INTERNAL_ERROR";
        case EVMC_REJECTED:
            return "EVMC_REJECTED";
        case EVMC_OUT_OF_MEMORY:
            return "EVMC_OUT_OF_MEMORY";
        default:
            return "none";
    };
}
