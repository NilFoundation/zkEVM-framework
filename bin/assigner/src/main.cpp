#include <expected>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <string>

#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_NO_DEPRECATED
#endif
#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED
#endif

#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <nil/crypto3/algebra/curves/bls12.hpp>
#include <nil/crypto3/algebra/curves/ed25519.hpp>
#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/bls12.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/ed25519.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/pallas.hpp>
#include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>
#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>

#include "zkevm_framework/assigner_runner/runner.hpp"
#include "zkevm_framework/assigner_runner/state_parser.hpp"
#include "zkevm_framework/assigner_runner/utils.hpp"
#include "zkevm_framework/data_types/account.hpp"
#include "zkevm_framework/data_types/block.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

template<typename BlueprintFieldType>
int curve_dependent_main(const std::string& input_block_file_name,
                         const std::string& account_storage_config_name,
                         const std::string& assignment_table_file_name,
                         const std::optional<OutputArtifacts>& artifacts,
                         boost::log::trivial::severity_level log_level,
                         std::vector<std::array<std::size_t, 4>>& column_sizes) {
    using ArithmetizationType =
        nil::crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>;

    // init current account storage
    evmc::accounts account_storage;
    if (!account_storage_config_name.empty()) {
        auto init_err = init_account_storage(account_storage, account_storage_config_name);
        if (init_err) {
            std::cerr << init_err.value() << std::endl;
            return -1;
        }
    }

    single_thread_runner<BlueprintFieldType> runner(account_storage, column_sizes, log_level);

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

    std::optional<std::string> run_err =
        runner.run(input_block, assignment_table_file_name, artifacts);
    if (run_err.has_value()) {
        std::cerr << "Assigner run failed: " << run_err.value() << std::endl;
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
            ("output-text", boost::program_options::value<std::string>(), "Output assignment table in readable format. "
                                                                          "Filename or `-` for stdout. "
                                                                          "Using this enables options --tables, --rows, --columns")
            ("tables", boost::program_options::value<std::string>(), "Assignment table indices to output. "
                                                                     "Format is --tables N|N-|-N|N-M(,N|N-|-N|N-M)*. "
                                                                     "If not specified, outputs all generated tables")
            ("rows", boost::program_options::value<std::string>(), "Range of rows of the table to output. "
                                                                   "Format is --rows N|N-|-N|N-M(,N|N-|-N|N-M)*. "
                                                                   "If not specified, outputs all rows")
            ("columns", boost::program_options::value<std::vector<std::string>>(), "Range of columns of the table to output. "
                                                                                   "Format is --columns <name>N|N-|-N|N-M(,N|N-|-N|N-M)*, where <name> is public_input|witness|constant|selector."
                                                                                   "If not specified, outputs all columns. "
                                                                                   "May be provided multiple times with different column types")
            ("input-assignment-tables,i", boost::program_options::value<std::string>(), "Assignment table input files")
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

    std::optional<OutputArtifacts> artifacts = std::nullopt;
    if (vm.count("output-text")) {
        auto maybe_artifacts = OutputArtifacts::from_program_options(vm);
        if (!maybe_artifacts.has_value()) {
            std::cerr << maybe_artifacts.error() << std::endl;
            std::cout << options_desc << std::endl;
            return 1;
        }
        artifacts = maybe_artifacts.value();
    }

    if (vm.count("input-assignment-tables")) {
        input_assignment_table_file_name = vm["input-assignment-tables"].as<std::string>();
    } else {
        input_assignment_table_file_name = "";
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
                artifacts, log_options[log_level], column_sizes);
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
                artifacts, log_options[log_level], column_sizes);
            break;
        }
    };

    return 0;
}
