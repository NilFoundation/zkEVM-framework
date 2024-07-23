/**
 * @file checks.hpp
 *
 * @brief This file defines functions and types for check assignment tables.
 */

#ifndef ZKEMV_FRAMEWORK_LIBS_CHECKS_INCLUDE_CHECKS_HPP_
#define ZKEMV_FRAMEWORK_LIBS_CHECKS_INCLUDE_CHECKS_HPP_

#include <nil/blueprint/blueprint/plonk/assignment.hpp>
#include <nil/blueprint/blueprint/plonk/circuit.hpp>
#include <nil/blueprint/utils/connectedness_check.hpp>
#include <nil/blueprint/utils/satisfiability_check.hpp>
#include <nil/blueprint/zkevm/bytecode.hpp>

template<typename BlueprintFieldType>
using ArithmetizationType = nil::crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>;

template<typename BlueprintFieldType>
bool check_connectedness(
    const nil::blueprint::circuit<
        ArithmetizationType<BlueprintFieldType>>& bp,
    const nil::blueprint::assignment<
        ArithmetizationType<BlueprintFieldType>>& assignment,
    const std::vector<
        nil::crypto3::zk::snark::plonk_variable<typename BlueprintFieldType::value_type>>&
        instance_input,
    const std::vector<
        nil::crypto3::zk::snark::plonk_variable<typename BlueprintFieldType::value_type>>&
        component_result,
    size_t start_row, size_t component_rows_amount,
    nil::blueprint::connectedness_check_type connectedness_check) {
    if (assignment.rows_amount() - start_row != component_rows_amount) {
        std::cerr << "ERROR: wrong rows ampunt: expected " << component_rows_amount << ", real "
                  << assignment.rows_amount() - start_row << "\n";
        return false;
    }
    const std::size_t rows_after_component_batching =
        assignment.finalize_component_batches(bp, start_row + component_rows_amount);
    const std::size_t rows_after_const_batching =
        assignment.finalize_constant_batches(bp, 0, std::max<std::size_t>(start_row, 1));
    const std::size_t rows_after_batching =
        std::max(rows_after_component_batching, rows_after_const_batching);
    for (auto variable : component_result) {
        if (assignment.get_batch_variable_map().count(variable)) {
            variable.get() = assignment.get_batch_variable_map().at(variable);
        }
    }
    bool is_connected = nil::blueprint::check_connectedness(
        assignment, bp, instance_input, component_result, start_row,
        rows_after_batching - start_row, connectedness_check);
    if (!is_connected) {
        std::cerr << "ERROR: connectness violation\n";
        return false;
    }
    if (connectedness_check.t == nil::blueprint::connectedness_check_type::type::NONE) {
        std::cerr << "WARNING: Connectedness check is disabled.\n";
    }
    return true;
}

template<typename BlueprintFieldType>
bool is_satisfied(
    const nil::blueprint::circuit<
        ArithmetizationType<BlueprintFieldType>>& bp,
    const nil::blueprint::assignment<
        ArithmetizationType<BlueprintFieldType>>& assignment) {
    if (!nil::blueprint::is_satisfied(bp, assignment)) {
        std::cerr << "ERROR: is not satisfied\n";
        return false;
    }
    return true;
}

template<typename BlueprintFieldType>
bool check_bytecode_satisfied(
    nil::blueprint::assignment<ArithmetizationType<BlueprintFieldType>>& bytecode_table) {
    using component_type =
        nil::blueprint::components::zkevm_bytecode<ArithmetizationType<BlueprintFieldType>,
                                                   BlueprintFieldType>;

    // Prepare witness container to make an instance of the component
    typename component_type::manifest_type m = component_type::get_manifest();
    size_t witness_amount = *(m.witness_amount->begin());
    std::vector<std::uint32_t> witnesses(witness_amount);
    for (std::uint32_t i = 0; i < witness_amount; i++) {
        witnesses[i] = i;
    }

    constexpr size_t max_code_size = 24576;
    component_type component_instance = component_type(
        witnesses, std::array<std::uint32_t, 1>{0}, std::array<std::uint32_t, 1>{0}, max_code_size);

    nil::blueprint::circuit<nil::crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>>
        bp;
    auto lookup_tables = component_instance.component_lookup_tables();
    for (auto& [k, v] : lookup_tables) {
        bp.reserve_table(k);
    }

    // TODO: pass a proper public input here
    typename component_type::input_type input({}, {}, typename component_type::var());
    nil::blueprint::components::generate_circuit(component_instance, bp, bytecode_table, input, 0);
    return ::is_satisfied(bp, bytecode_table);
}

#endif  // ZKEMV_FRAMEWORK_LIBS_CHECKS_INCLUDE_CHECKS_HPP_
