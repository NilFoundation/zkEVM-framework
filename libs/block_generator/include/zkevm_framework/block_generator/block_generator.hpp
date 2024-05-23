#ifndef ZKEMV_FRAMEWORK_LIBS_BLOCK_GENERATOR_BLOCK_GENERATOR_HPP_
#define ZKEMV_FRAMEWORK_LIBS_BLOCK_GENERATOR_BLOCK_GENERATOR_HPP_

#include <boost/json/value.hpp>
#include <expected>
#include <istream>
#include <string>

#include "zkevm_framework/data_types/block.hpp"

namespace data_types::block_generator {

    // Construct Block from JSON config file
    std::expected<Block, std::string> generate_block(const boost::json::value& json_value);

}  // namespace data_types::block_generator

#endif  // ZKEMV_FRAMEWORK_LIBS_GENERATOR_GENERATOR_H_
