#ifndef ZKEMV_FRAMEWORK_LIBS_BLOCK_GENERATOR_BLOCK_EXTRACTOR_HPP_
#define ZKEMV_FRAMEWORK_LIBS_BLOCK_GENERATOR_BLOCK_EXTRACTOR_HPP_

#include <expected>
#include <istream>
#include <string>

#include "zkevm_framework/data_types/block.hpp"

namespace data_types {

    class block_extractor {
    public:
        block_extractor(std::string host, int port): m_host(host), m_port(port) {}
        std::expected<Block, std::string> get_block(uint64_t id);
    private:
        std::string m_host;
        int m_port;
    };

}  // namespace data_types

#endif  // ZKEMV_FRAMEWORK_LIBS_GENERATOR_EXTRACTOR_H_
