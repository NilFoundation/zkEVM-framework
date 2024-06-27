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
                         boost::log::trivial::severity_level log_level = boost::log::trivial::info)
        : m_log_level(log_level) {
        initialize_assignments(column_sizes);
    }

    /// @brief Execute one block
    std::optional<std::string> run(uint64_t shardId, uint64_t blockId,
                                   const std::string& assignment_table_file_name,
                                   const std::optional<OutputArtifacts>& artifacts);

    /// @brief Execute the block but write only to internal assignments field
    std::optional<std::string> fill_assignments(uint64_t shardId, uint64_t blockId);

    /// @brief Get reference to assignents
    const std::vector<nil::blueprint::assignment<ArithmetizationType>>& get_assignments() const;

  private:
    void initialize_assignments(const std::vector<std::array<std::size_t, 4>>& column_sizes);

    std::vector<nil::blueprint::assignment<ArithmetizationType>> m_assignments;
    boost::log::trivial::severity_level m_log_level;
};

#endif  // ZKEMV_FRAMEWORK_LIBS_ASSIGNER_RUNNER_INCLUDE_ZKEVM_FRAMEWORK_ASSIGNER_RUNNER_RUNNER_HPP_
