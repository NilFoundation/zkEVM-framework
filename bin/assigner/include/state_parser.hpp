#ifndef ZKEMV_FRAMEWORK_BIN_ASSIGNER_INCLUDE_STATE_PARSER_HPP_
#define ZKEMV_FRAMEWORK_BIN_ASSIGNER_INCLUDE_STATE_PARSER_HPP_

#include <istream>
#include <optional>
#include <string>

#include "vm_host.h"

std::optional<std::string> init_account_storage(evmc::accounts &res, std::istream &config);

#endif  // ZKEMV_FRAMEWORK_BIN_ASSIGNER_INCLUDE_STATE_PARSER_HPP_
