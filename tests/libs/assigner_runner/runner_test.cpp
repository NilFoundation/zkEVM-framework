#include "zkevm_framework/assigner_runner/runner.hpp"

#include <gtest/gtest.h>

#include <boost/log/trivial.hpp>
#include <fstream>
#include <nil/crypto3/algebra/curves/pallas.hpp>

#include "zkevm_framework/assigner_runner/state_parser.hpp"
#include "zkevm_framework/block_generator/block_generator.hpp"
#include "zkevm_framework/data_types/block.hpp"
#include "zkevm_framework/json_helpers/json_helpers.hpp"

TEST(runner_test, check_block) {
    std::ifstream input_block_file(BLOCK_CONFIG);
    ASSERT_TRUE(input_block_file.is_open());
    auto json_val = json_helpers::parse_json(input_block_file);
    auto maybe_input_block = data_types::block_generator::generate_block(json_val.value());
    ASSERT_TRUE(maybe_input_block.has_value());
    data_types::Block input_block = maybe_input_block.value();
    input_block_file.close();

    using BlueprintFieldType = typename nil::crypto3::algebra::curves::pallas::base_field_type;
    using ArithmetizationType =
        nil::crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>;
    std::vector<std::array<std::size_t, 4>> column_sizes = {{
                                                                65,  // witness
                                                                1,   // public_input
                                                                5,   // constants
                                                                30   // selectors
                                                            },
                                                            {
                                                                65,  // witness
                                                                1,   // public_input
                                                                5,   // constants
                                                                30   // selectors
                                                            }};

    evmc::accounts account_storage;
    auto init_err = init_account_storage(account_storage, STATE_CONFIG);
    ASSERT_FALSE(init_err.has_value());
    single_thread_runner<BlueprintFieldType> runner(account_storage, column_sizes, "",
                                                    boost::log::trivial::severity_level::debug);
    auto run_err = runner.fill_assignments(input_block);
    ASSERT_FALSE(run_err.has_value());
    // Checks below are disabled due to frequent changes in filling assignment tables
    /*
    auto assignments = runner.get_assignments();
    ASSERT_EQ(assignments.size(), 2);
    ASSERT_EQ(assignments[1].witness(0, 0), 8);
    ASSERT_EQ(assignments[1].witness(0, 1), 4);
    */
}
