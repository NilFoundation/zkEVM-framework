#ifndef ZKEMV_FRAMEWORK_LIBS_PRESET_PRESET_HPP_
#define ZKEMV_FRAMEWORK_LIBS_PRESET_PRESET_HPP_

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <istream>
#include <nil/blueprint/blueprint/plonk/assignment.hpp>
#include <optional>
#include <string>

#include "zkevm_framework/preset/bytecode.hpp"

template<typename ArithmetizationType>
struct zkevm_circuits {
    std::vector<std::string> m_default_names = {"bytecode"};
    std::vector<std::string> m_names;
    nil::blueprint::circuit<ArithmetizationType> m_bytecode_circuit;
    const std::vector<std::string>& get_circuit_names() {
        return m_names.size() > 0 ? m_names : m_default_names;
    }
};

template<typename BlueprintFieldType>
std::optional<std::string> initialize_circuits(
    zkevm_circuits<nil::crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>>& circuits,
    std::vector<nil::blueprint::assignment<
        nil::crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>>>& assignments) {
    const auto& circuit_names = circuits.get_circuit_names();
    BOOST_LOG_TRIVIAL(debug) << "Number assignment tables = " << circuit_names.size() << "\n";
    nil::crypto3::zk::snark::plonk_table_description<BlueprintFieldType> empty_desc(0, 0, 0, 0);
    for (size_t i = 0; i < circuit_names.size(); i++) {
        assignments.push_back(empty_desc);
    }
    for (const auto& circuit_name : circuit_names) {
        BOOST_LOG_TRIVIAL(debug) << "Initialize circuit = " << circuit_name << "\n";
        if (circuit_name == "bytecode") {
            auto& bytecode_table =
                assignments[nil::evm_assigner::assigner<BlueprintFieldType>::BYTECODE_TABLE_INDEX];
            auto err = initialize_bytecode_circuit(circuits.m_bytecode_circuit, bytecode_table);
            if (err) {
                return err;
            }
        }
    }
    return {};
}

#endif  // ZKEMV_FRAMEWORK_LIBS_PRESET_PRESET_HPP_
