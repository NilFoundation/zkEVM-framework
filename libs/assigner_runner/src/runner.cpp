#include "zkevm_framework/assigner_runner/runner.hpp"

#include <assigner.hpp>
#include <nil/crypto3/algebra/curves/bls12.hpp>
#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <optional>
#include <sstream>
#include <string>

#include "zkevm_framework/assigner_runner/utils.hpp"
#include "zkevm_framework/assigner_runner/write_assignments.hpp"
#include "zkevm_framework/data_types/account.hpp"
#include "zkevm_framework/data_types/transaction.hpp"

template<typename BlueprintFieldType>
void single_thread_runner<BlueprintFieldType>::initialize_assignments(
    const std::vector<std::array<std::size_t, 4>>& column_sizes) {
    // initialize assignment tables
    for (const auto& table_column_sizes : column_sizes) {
        nil::crypto3::zk::snark::plonk_table_description<BlueprintFieldType> desc(
            table_column_sizes[0], table_column_sizes[1], table_column_sizes[2],
            table_column_sizes[3]);
        m_assignments.emplace_back(desc);
    }
}

template<typename BlueprintFieldType>
std::optional<std::string> single_thread_runner<BlueprintFieldType>::run(
    const unsigned char* input_block_data, size_t input_block_size,
    const std::string& assignment_table_file_name,
    const std::optional<OutputArtifacts>& artifacts) {
    data_types::bytes block_data;
    block_data.resize(input_block_size);
    for (size_t i = 0; i < input_block_size; i++) {
        block_data[i] = static_cast<std::byte>(input_block_data[i]);
    }

    auto maybe_input_block = data_types::Block::deserialize(block_data);
    if (!maybe_input_block.has_value()) {
        std::ostringstream error;
        error << maybe_input_block.error();
        return error.str();
    }

    auto input_block = maybe_input_block.value();
    return run(input_block, assignment_table_file_name, artifacts);
}

template<typename BlueprintFieldType>
std::optional<std::string> single_thread_runner<BlueprintFieldType>::run(
    const data_types::Block& input_block, const std::string& assignment_table_file_name,
    const std::optional<OutputArtifacts>& artifacts) {
    fill_assignments(input_block);

    BOOST_LOG_TRIVIAL(debug) << "print assignment tables " << m_assignments.size() << "\n";

    using Endianness = nil::marshalling::option::big_endian;

    std::optional<std::string> err =
        write_binary_assignments<Endianness, ArithmetizationType, BlueprintFieldType>(
            m_assignments, assignment_table_file_name);
    if (err) {
        return err;
    }

    if (artifacts.has_value()) {
        std::optional<std::string> err =
            write_output_artifacts<Endianness, ArithmetizationType, BlueprintFieldType>(
                m_assignments, artifacts.value());
        if (err) {
            return err;
        }
    }

    return {};
}

template<typename BlueprintFieldType>
std::optional<std::string> single_thread_runner<BlueprintFieldType>::fill_assignments(
    const data_types::Block& input_block) {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= m_log_level);
    std::ostringstream error;

    // create assigner instance
    auto assigner_ptr =
        std::make_shared<nil::blueprint::assigner<BlueprintFieldType>>(m_assignments);

    // get header of the current block
    const auto block_header = input_block.m_currentBlock;

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

    VMHost host(tx_context, m_account_storage, assigner_ptr);
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
                error << "Transaction  with id " << input_msg.m_transaction.m_id << " is not found";
                return error.str();
            }
        } else {
            error << "Account block for address " << to_str(input_msg.m_info.m_dst)
                  << "is not found";
            return error.str();
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
            error << "Failed copy transaction" << transaction.m_id
                  << " code: expected size = " << code.size() << ", real = " << count;
            return error.str();
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

        auto res = nil::blueprint::evaluate(host_interface, ctx, rev, &msg, code.data(),
                                            code.size(), assigner_ptr);

        BOOST_LOG_TRIVIAL(debug) << "evaluate result = " << to_str(res.status_code) << "\n";
        if (res.status_code == EVMC_SUCCESS) {
            BOOST_LOG_TRIVIAL(debug) << "create_address = " << to_str(res.create_address) << "\n"
                                     << "gas_left = " << res.gas_left << "\n"
                                     << "gas_refund = " << res.gas_refund << "\n"
                                     << "output size = " << res.output_size << "\n";
        }
    }
    return {};
}

template<typename BlueprintFieldType>
const std::vector<nil::blueprint::assignment<
    typename single_thread_runner<BlueprintFieldType>::ArithmetizationType>>&
single_thread_runner<BlueprintFieldType>::get_assignments() const {
    return m_assignments;
}

// Instantiate runner for required field types

using pallas_base_field = typename nil::crypto3::algebra::curves::pallas::base_field_type;
template class single_thread_runner<pallas_base_field>;

using bls_base_field = typename nil::crypto3::algebra::fields::bls12_base_field<381>;
template class single_thread_runner<bls_base_field>;
