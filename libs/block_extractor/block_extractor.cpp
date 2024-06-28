#include "zkevm_framework/block_extractor/block_extractor.hpp"

#include "zkevm_framework/data_types/account_block.hpp"
#include "zkevm_framework/data_types/base.hpp"
#include "zkevm_framework/data_types/block.hpp"
#include "zkevm_framework/data_types/block_header.hpp"
#include "zkevm_framework/data_types/message.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

//#define CPPHTTPLIB_ZLIB_SUPPORT
#include <httplib.h>

namespace data_types {
    std::expected<Block, std::string> block_extractor::get_block(uint64_t id) {
        httplib::Client cli(m_host, m_port);
        httplib::Headers headers = {
            //{ "Accept-Encoding", "gzip" }
        };
        std::string body = "{\"id\":1,\"jsonrpc\":\"2.0\",\"method\":\"eth_getBlockByNumber\",\"params\":[1,\"latest\",true]}";
        if (auto res = cli.Post("/", headers, body.c_str(), body.size(), "application/json")) {
            std::cout << res->status << "\n";
            std::cout << res->get_header_value("Content-Type") << "\n";
            std::cout << res->body << "\n";
        } else {
            std::cout << "error code: " << res.error() << "\n";
        }
        data_types::Block res;
        //return std::unexpected<std::string>(*validation_err);
        return res;
    }
}  // namespace data_types::block_generator
