#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <boost/program_options.hpp>
#include <expected>
#include <fstream>
#include <iostream>
#include <string>

#include "zkevm_framework/block_generator/block_generator.hpp"
#include "zkevm_framework/data_types/block.hpp"

int main(int argc, char* argv[]) {
    boost::program_options::options_description options_desc("Block generator");

    // clang-format off
    options_desc.add_options()("help,h", "Display help message")
            ("version,v", "Display version")
            ("config-file,i", boost::program_options::value<std::string>(), "Configuration of the block in JSON format")
            ("output-file,o", boost::program_options::value<std::string>(), "Generated block");
            ;
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
#ifdef GENERATOR_VERSION
#define xstr(s) str(s)
#define str(s) #s
        std::cout << xstr(GENERATOR_VERSION) << std::endl;
#else
        std::cout << "Version is not defined" << std::endl;
#endif
        return 0;
    }

    if (!vm.count("config-file")) {
        std::cerr << "Config file was not provided" << std::endl;
        return 1;
    }
    const auto& input_file_name = vm["config-file"].as<std::string>();
    std::string output_file_name = "block.out";
    if (vm.count("output-file")) {
        output_file_name = vm["output-file"].as<std::string>();
    }

    std::ifstream input_file(input_file_name.c_str());
    if (!input_file.is_open()) {
        std::cerr << "Could not open the config file " << input_file_name << std::endl;
        return 1;
    }
    std::expected<boost::json::value, std::string> parsed_json =
        data_types::block_generator::parse_json(input_file);
    if (!parsed_json) {
        std::cerr << "Parsing of JSON config file failed: " << parsed_json.error() << std::endl;
        return 1;
    }

    std::expected<data_types::Block, std::string> block =
        data_types::block_generator::generate_block(parsed_json.value());
    if (!block) {
        std::cerr << "Error while parsing config file: " << block.error() << std::endl;
        return 1;
    }

    std::ofstream output_file(output_file_name);
    std::expected<void, data_types::SerializationError> serialization_result =
        block->serialize(output_file);
    if (!serialization_result) {
        std::cerr << serialization_result.error();
        return 1;
    }
    return 0;
}
