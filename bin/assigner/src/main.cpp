#include <assert.h>

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include <evmone/evmone.h>

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <nil/blueprint/assert.hpp>
#include <nil/blueprint/assigner.hpp>
#include <nil/crypto3/algebra/curves/bls12.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>
#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/bls12.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/ed25519.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/pallas.hpp>
#include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>
#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>

#include "state_parser.hpp"
#include "utils.h"
#include "vm_host.h"
#include "write_assignments.hpp"
#include "zkevm_framework/data_types/account.hpp"
#include "zkevm_framework/data_types/block.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

template<typename BlueprintFieldType>
int curve_dependent_main(const std::string& input_block_file_name,
                         const std::string& account_storage_config_name,
                         const std::string& assignment_table_file_name,
                         boost::log::trivial::severity_level log_level,
                         std::vector<std::array<std::size_t, 4>>& column_sizes) {
    using ArithmetizationType =
        nil::crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>;

    // initialize assignment tables
    std::vector<nil::blueprint::assignment<ArithmetizationType>> assignments;
    for (const auto& table_column_sizes : column_sizes) {
        nil::crypto3::zk::snark::plonk_table_description<BlueprintFieldType> desc(
            table_column_sizes[0], table_column_sizes[1], table_column_sizes[2],
            table_column_sizes[3]);
        assignments.emplace_back(desc);
    }

    // TODO somehow fill by initial values (public_input, ... )

    // create assigner instance
    nil::blueprint::assigner<BlueprintFieldType> assigner_instance(assignments);

    std::ifstream input_block_file(input_block_file_name.c_str(),
                                   std::ios_base::binary | std::ios_base::in);
    if (!input_block_file.is_open()) {
        std::cerr << "Could not open the file - '" << input_block_file_name << "'" << std::endl;
        return -1;
    }
    auto maybe_input_block = data_types::Block::deserialize(input_block_file);
    if (!maybe_input_block.has_value()) {
        std::cerr << "Could not read - '" << input_block_file_name << "'"
                  << ": " << maybe_input_block.error() << std::endl;
        input_block_file.close();
        return -1;
    }
    auto input_block = maybe_input_block.value();
    input_block_file.close();

    // set log level
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= log_level);

    // get header of the current block
    const auto block_header = input_block.m_currentBlock;

    // create EVM instance
    auto vm = evmc_create_evmone();
    evmc_revision rev = {};

    const evmc_address empty_addr = to_evmc_address({0});

    BOOST_LOG_TRIVIAL(debug) << "Input Block:\n"
                             << "  coinbase = " << to_str(block_header.m_coinbase) << "\n"
                             << "  block number = " << block_header.m_number << "\n"
                             << "  timestamp = " << block_header.m_timestamp << "\n"
                             << "  gas limit = " << block_header.m_gasLimit << "\n"
                             << "  prev randao = " << block_header.m_prevrandao << "\n"
                             << "  chain id = " << block_header.m_chain_id << "\n"
                             << "  base fee = " << block_header.m_basefee << "\n"
                             << "  blob base fee = " << block_header.m_blob_basefee << "\n"
                             << "\n";
    // transaction and block data for execution
    struct evmc_tx_context tx_context = {
        // per transaction value
        .tx_gas_price = {{0}}, /**< The transaction gas price. */

        // per account block value
        .tx_origin = empty_addr, /**< The transaction origin account. */

        .block_coinbase = to_evmc_address(block_header.m_coinbase), /**< The miner of the block. */
        .block_number = (int64_t)block_header.m_number,             /**< The block number. */
        .block_timestamp = (int64_t)block_header.m_timestamp,       /**< The block timestamp. */
        .block_gas_limit = (int64_t)block_header.m_gasLimit,        /**< The block gas limit. */
        .block_prev_randao =
            to_uint256be(block_header.m_prevrandao), /**< The block previous RANDAO (EIP-4399). */
        .chain_id = to_uint256be(block_header.m_chain_id), /**< The blockchain's ChainID. */
        .block_base_fee = to_uint256be(
            block_header.m_basefee), /**< The block base fee per gas (EIP-1559, EIP-3198). */
        .blob_base_fee =
            to_uint256be(block_header.m_blob_basefee), /**< The blob base fee (EIP-7516). */
        .blob_hashes = nullptr,                        /**< The array of blob hashes (EIP-4844). */
        .blob_hashes_count = 0,                        /**< The number of blob hashes (EIP-4844). */
    };

    // default interface for access to the host
    const struct evmc_host_interface* host_interface = &evmc::Host::get_interface();

    // init current account storage
    evmc::accounts account_storage;
    if (!account_storage_config_name.empty()) {
        std::ifstream asc_stream(account_storage_config_name);
        if (!asc_stream.is_open()) {
            std::cerr << "Could not open the account storage config: '"
                      << account_storage_config_name << "'" << std::endl;
            return -1;
        }
        auto init_err = init_account_storage(account_storage, asc_stream);
        if (init_err) {
            std::cerr << init_err.value() << std::endl;
            return -1;
        }

        if (log_level <= boost::log::trivial::debug) {
            BOOST_LOG_TRIVIAL(debug)
                << "Account storage initialized with " << account_storage.size() << " accounts: \n";
            for (const auto& [addr, acc] : account_storage) {
                BOOST_LOG_TRIVIAL(debug) << "\tAddress: " << to_str(addr) << '\n'
                                         << "\tBalance: " << to_str(acc.balance) << '\n';
                if (!acc.code.empty()) {
                    BOOST_LOG_TRIVIAL(debug) << "\tCode: " << to_str(acc.code);
                }
                if (!acc.storage.empty()) {
                    BOOST_LOG_TRIVIAL(debug) << "\tStorage:\n";
                    for (const auto& [key, value] : acc.storage) {
                        BOOST_LOG_TRIVIAL(debug)
                            << "[ " << to_str(key) << " ] = " << to_str(value) << "\n";
                    }
                }
                BOOST_LOG_TRIVIAL(debug) << std::endl;
            }
        }
    }

    VMHost host(tx_context, account_storage, assigner_instance.get_handler());
    struct evmc_host_context* ctx = host.to_context();
    const std::vector<data_types::AccountBlock>& accountBlocks = input_block.m_accountBlocks;

    for (const auto& input_msg : input_block.m_inputMsgs) {
        const evmc_address origin_addr = to_evmc_address(input_msg.m_info.m_src);
        tx_context.tx_origin = origin_addr;

        BOOST_LOG_TRIVIAL(debug) << "process CALL message\n  from "
                                 << to_str(input_msg.m_info.m_src) << " to "
                                 << to_str(input_msg.m_info.m_dst) << "\n";

        // check if transaction in block
        const auto acc_block_it =
            std::find_if(accountBlocks.begin(), accountBlocks.end(),
                         [input_msg](const data_types::AccountBlock& acc) {
                             return acc.m_accountAddress == input_msg.m_info.m_dst;
                         });
        if (acc_block_it != accountBlocks.end()) {
            const std::vector<data_types::Transaction>& transactions = acc_block_it->m_transactions;
            const auto transaction_it =
                std::find_if(transactions.begin(), transactions.end(),
                             [input_msg](const data_types::Transaction& t) {
                                 return t.m_id == input_msg.m_transaction.m_id;
                             });
            if (transaction_it == transactions.end()) {
                std::cerr << "Not found transaction" << std::endl;
                return -1;
            }
        } else {
            std::cerr << "Not found account block" << std::endl;
            return -1;
        }

        const auto& transaction = input_msg.m_transaction;
        if (transaction.m_type != data_types::Transaction::Type::ContractCreation &&
            transaction.m_type != data_types::Transaction::Type::MessageCall) {
            BOOST_LOG_TRIVIAL(debug) << "skip transaction " << transaction.m_id << "("
                                     << to_str(transaction.m_type) << "). Nothing to do\n";
            continue;
        }
        // set tansaction related fields
        tx_context.tx_gas_price = to_uint256be(transaction.m_gasPrice);
        data_types::bytes transaction_code = transaction.m_data;
        std::vector<uint8_t> code(transaction_code.size());
        size_t count = 0;
        std::for_each(transaction_code.begin(), transaction_code.end(),
                      [&count, &code](const std::byte& v) {
                          code[count] = to_integer<uint8_t>(v);
                          count++;
                      });
        if (count != code.size()) {
            std::cerr << "Failed copy transaction" << transaction.m_id
                      << " code: expected size = " << code.size() << ", real = " << count
                      << std::endl;
            return -1;
        }

        // init messge associated with transaction
        const evmc_uint256be value = to_uint256be(transaction.m_value);
        const evmc_address sender_addr = to_evmc_address(transaction.m_sender);
        const evmc_address recipient_addr = to_evmc_address(transaction.m_receiveAddress);
        const int64_t gas = transaction.m_gas;
        const uint8_t input[] = "";
        struct evmc_message msg = {.kind = evmc_msg_kind(transaction.m_type),
                                   .flags = uint32_t{EVMC_STATIC},
                                   .depth = 0,
                                   .gas = gas,
                                   .recipient = recipient_addr,
                                   .sender = sender_addr,
                                   .input_data = input,
                                   .input_size = sizeof(input),
                                   .value = value,
                                   .create2_salt = {0},
                                   .code_address = origin_addr};

        BOOST_LOG_TRIVIAL(debug) << "evaluate transaction " << transaction.m_id << "\n"
                                 << "  type = " << to_str(transaction.m_type) << "\n"
                                 << "  nonce = " << transaction.m_nonce << "\n"
                                 << "  value = " << transaction.m_value << "\n"
                                 << "  gas price = " << transaction.m_gasPrice << "\n"
                                 << "  gas = " << transaction.m_gas << "\n"
                                 << "  code size = " << transaction.m_data.size() << "\n";

        auto res = assigner_instance.evaluate(vm, host_interface, ctx, rev, &msg, code.data(),
                                              code.size());

        BOOST_LOG_TRIVIAL(debug) << "evaluate result = " << to_str(res.status_code) << "\n";
        if (res.status_code == EVMC_SUCCESS) {
            BOOST_LOG_TRIVIAL(debug) << "create_address = " << to_str(res.create_address) << "\n"
                                     << "gas_left = " << res.gas_left << "\n"
                                     << "gas_refund = " << res.gas_refund << "\n"
                                     << "output size = " << res.output_size << "\n";
        }
    }

    BOOST_LOG_TRIVIAL(debug) << "print assignment tables " << assignments.size() << "\n";

    using Endianness = nil::marshalling::option::big_endian;

    bool err = write_binary_assignments<Endianness, ArithmetizationType, BlueprintFieldType>(
        assignments, assignment_table_file_name);
    if (err) {
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    boost::program_options::options_description options_desc("zkEVM1 assigner");

    // clang-format off
    options_desc.add_options()("help,h", "Display help message")
            ("version,v", "Display version")
            ("assignment-tables,t", boost::program_options::value<std::string>(), "Assignment table output files")
            ("input-block,b", boost::program_options::value<std::string>(), "Block input files")
            ("account-storage,s", boost::program_options::value<std::string>(), "Account storage config file")
            ("elliptic-curve-type,e", boost::program_options::value<std::string>(), "Native elliptic curve type (pallas, vesta, ed25519, bls12381)")
            ("log-level,l", boost::program_options::value<std::string>(), "Log level (trace, debug, info, warning, error, fatal)");
    // clang-format on

    boost::program_options::variables_map vm;
    try {
        boost::program_options::store(
            boost::program_options::command_line_parser(argc, argv).options(options_desc).run(),
            vm);
        boost::program_options::notify(vm);
    } catch (const boost::program_options::unknown_option& e) {
        std::cerr << "Invalid command line argument: " << e.what() << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    if (vm.count("help")) {
        std::cout << options_desc << std::endl;
        return 0;
    }

    if (vm.count("version")) {
#ifdef ASSIGNER_VERSION
#define xstr(s) str(s)
#define str(s) #s
        std::cout << xstr(ASSIGNER_VERSION) << std::endl;
#else
        std::cout << "Version is not defined" << std::endl;
#endif
        return 0;
    }

    std::string assignment_table_file_name;
    std::string input_assignment_table_file_name;
    std::string input_block_file_name;
    std::string account_storage_config_name;
    std::string elliptic_curve;
    std::string log_level;

    if (vm.count("assignment-tables")) {
        assignment_table_file_name = vm["assignment-tables"].as<std::string>();
    } else {
        std::cerr << "Invalid command line argument - assignment table file name is not specified"
                  << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    if (vm.count("input-block")) {
        input_block_file_name = vm["input-block"].as<std::string>();
    } else {
        std::cerr << "Invalid command line argument - input block file name is not specified"
                  << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    if (vm.count("account-storage")) {
        account_storage_config_name = vm["account-storage"].as<std::string>();
    }

    if (vm.count("elliptic-curve-type")) {
        elliptic_curve = vm["elliptic-curve-type"].as<std::string>();
    } else {
        elliptic_curve = "pallas";
    }

    if (vm.count("log-level")) {
        log_level = vm["log-level"].as<std::string>();
    } else {
        log_level = "info";
    }
    // We use Boost log trivial severity levels, these are string representations of their names
    std::map<std::string, boost::log::trivial::severity_level> log_options{
        {"trace", boost::log::trivial::trace}, {"debug", boost::log::trivial::debug},
        {"info", boost::log::trivial::info},   {"warning", boost::log::trivial::warning},
        {"error", boost::log::trivial::error}, {"fatal", boost::log::trivial::fatal}};

    if (log_options.find(log_level) == log_options.end()) {
        std::cerr << "Invalid command line argument -l (log level): " << log_level << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    std::map<std::string, int> curve_options{
        {"pallas", 0},
        {"vesta", 1},
        {"ed25519", 2},
        {"bls12381", 3},
    };

    if (curve_options.find(elliptic_curve) == curve_options.end()) {
        std::cerr << "Invalid command line argument -e (Native elliptic curve type): "
                  << elliptic_curve << std::endl;
        std::cout << options_desc << std::endl;
        return 1;
    }

    std::vector<std::array<std::size_t, 4>> column_sizes = {{
                                                                15,  // witness
                                                                1,   // public_input
                                                                5,   // constants
                                                                30   // selectors
                                                            },
                                                            {
                                                                15,  // witness
                                                                1,   // public_input
                                                                5,   // constants
                                                                30   // selectors
                                                            }};

    switch (curve_options[elliptic_curve]) {
        case 0: {
            return curve_dependent_main<
                typename nil::crypto3::algebra::curves::pallas::base_field_type>(
                input_block_file_name, account_storage_config_name, assignment_table_file_name,
                log_options[log_level], column_sizes);
            break;
        }
        case 1: {
            std::cerr << "vesta curve based circuits are not supported yet\n";
            break;
        }
        case 2: {
            std::cerr << "ed25519 curve based circuits are not supported yet\n";
            break;
        }
        case 3: {
            return curve_dependent_main<
                typename nil::crypto3::algebra::fields::bls12_base_field<381>>(
                input_block_file_name, account_storage_config_name, assignment_table_file_name,
                log_options[log_level], column_sizes);
            break;
        }
    };

    return 0;
}
