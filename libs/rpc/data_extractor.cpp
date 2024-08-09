#include "zkevm_framework/rpc/data_extractor.hpp"

// #define CPPHTTPLIB_ZLIB_SUPPORT
#include <httplib.h>

namespace data_types {
    std::optional<std::string> data_extractor::get_block_with_messages(
        const std::string& blockHash, std::stringstream& block_data) const {
        httplib::Client cli(m_host, m_port);
        httplib::Headers headers = {};
        std::string body =
            "{\"id\":1,\"jsonrpc\":\"2.0\",\"method\":\"debug_getBlockByHash\",\"params\":[";
        body += std::to_string(m_shard_id);
        body += ",\"" + blockHash + "\",true]}";
        std::cout << body << "\n";
        if (auto res = cli.Post("/", headers, body.c_str(), body.size(), "application/json")) {
            std::cout << res->status << "\n";
            std::cout << res->get_header_value("Content-Type") << "\n";
            std::cout << res->body << "\n";

            block_data << res->body;
            return {};
        } else {
            return "Response error code: " + httplib::to_string(res.error());
        }
    }

    std::optional<std::string> data_extractor::get_account_with_storage(
        const std::string& address, const std::string& blockHash,
        std::stringstream& account_data) const {
        httplib::Client cli(m_host, m_port);
        httplib::Headers headers = {};
        // get account proof and storage
        std::string body =
            "{\"id\":1,\"jsonrpc\":\"2.0\",\"method\":\"debug_getContract\",\"params\":[";
        body += "\"" + address + "\"";
        body += ",\"" + blockHash + "\"]}";
        std::cout << body << "\n";
        if (auto res = cli.Post("/", headers, body.c_str(), body.size(), "application/json")) {
            std::cout << res->status << "\n";
            std::cout << res->get_header_value("Content-Type") << "\n";
            std::cout << res->body << "\n";

            account_data << res->body;
            return {};
        } else {
            return "Response error code: " + httplib::to_string(res.error());
        }
    }
}  // namespace data_types
