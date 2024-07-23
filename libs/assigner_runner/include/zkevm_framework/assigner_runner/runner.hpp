#ifndef ZKEMV_FRAMEWORK_LIBS_ASSIGNER_RUNNER_INCLUDE_ZKEVM_FRAMEWORK_ASSIGNER_RUNNER_RUNNER_HPP_
#define ZKEMV_FRAMEWORK_LIBS_ASSIGNER_RUNNER_INCLUDE_ZKEVM_FRAMEWORK_ASSIGNER_RUNNER_RUNNER_HPP_

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <nil/blueprint/blueprint/plonk/assignment.hpp>

#include "output_artifacts.hpp"
#include "vm_host.hpp"
#include "zkevm_framework/assigner_runner/utils.hpp"
#include "zkevm_framework/data_types/block.hpp"

template<typename BlueprintFieldType>
class single_thread_runner {
  public:
    using ArithmetizationType =
        nil::crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>;

    /// @brief Initialize runner with empty account storage
    single_thread_runner(const std::vector<std::array<std::size_t, 4>>& column_sizes,
                         const std::string& target_circuit = "",
                         boost::log::trivial::severity_level log_level = boost::log::trivial::info)
        : m_target_circuit(target_circuit), m_log_level(log_level) {
        initialize_assignments(column_sizes);
    }

    /// @brief Initialize runner with predefined account storage
    single_thread_runner(const evmc::accounts& account_storage,
                         const std::vector<std::array<std::size_t, 4>>& column_sizes,
                         const std::string& target_circuit = "",
                         boost::log::trivial::severity_level log_level = boost::log::trivial::info)
        : m_account_storage(account_storage),
          m_target_circuit(target_circuit),
          m_log_level(log_level) {
        initialize_assignments(column_sizes);
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

    /// @brief Execute one block presented as native unsigned char array
    std::optional<std::string> run(const unsigned char* input_block_data, size_t input_block_size,
                                   const std::string& assignment_table_file_name,
                                   const std::optional<OutputArtifacts>& artifacts);

    /// @brief Execute one block presented as data_types::Block structure
    std::optional<std::string> run(const data_types::Block& input_block,
                                   const std::string& assignment_table_file_name,
                                   const std::optional<OutputArtifacts>& artifacts);

    /// @brief Execute the block but write only to internal assignments field
    std::optional<std::string> fill_assignments(const data_types::Block& input_block);

    /// @brief Get reference to assignents
    std::vector<nil::blueprint::assignment<ArithmetizationType>>& get_assignments();

  private:
    void initialize_assignments(const std::vector<std::array<std::size_t, 4>>& column_sizes);

    std::vector<nil::blueprint::assignment<ArithmetizationType>> m_assignments;
    evmc::accounts m_account_storage;
    std::string m_target_circuit;
    boost::log::trivial::severity_level m_log_level;
};

#endif  // ZKEMV_FRAMEWORK_LIBS_ASSIGNER_RUNNER_INCLUDE_ZKEVM_FRAMEWORK_ASSIGNER_RUNNER_RUNNER_HPP_
