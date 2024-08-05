#!/usr/bin/env python3

import argparse
from contextlib import contextmanager
import hashlib
import json
import jsonschema
import logging
from pathlib import Path
import random
import re
import os
import subprocess
from time import sleep
from typing import Dict

# Interface for launching nil cluster RPC server
class ClusterProcess:
    def __init__(self):
        logging.info("Launching nil cluster")
        self.process = subprocess.Popen(["nil", "run"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        # TODO: Ensure that the server is up at the time of a first request
        sleep(4)
    def shutdown(self):
        logging.info("Terminating nil cluster")
        self.process.terminate()

    # Implementation of ContextManager interface (to use inside "with")
    def __enter__(self):
        return self

    def __exit__(self, *exit_args):
        self.shutdown()

# Interface for usage of nil_cli
class NilCLI:
    def __init__(self, cli_config_name=None):
        self.config_path = cli_config_name

    def run_cli(self, args: list[str], pattern=None) -> str:
        full_arg_list = ["nil_cli"]
        if self.config_path is not None:
            full_arg_list += ["-c", self.config_path]
        full_arg_list += args
        logging.info("Launching " + " ".join(full_arg_list))
        p = subprocess.run(full_arg_list, capture_output=True)
        if p.returncode != 0:
            raise RuntimeError(p.stderr)
        output = p.stdout.decode()
        # result of "config show" command printed to stderr
        if not output:
            output = p.stderr.decode()
        logging.debug(output)
        if pattern is None:
            return output
        a = re.search(pattern, output)
        if a is None:
            raise RuntimeError(p.stderr)
        return a.group(1)

    def wallet_address(self) -> str:
        wallet_pattern = "Wallet address: (0x[0-9a-fA-F]+)"
        return self.run_cli(["wallet", "info"], pattern=wallet_pattern)

    def new_wallet(self) -> str:
        wallet_pattern = "New wallet address: (0x[0-9a-fA-F]+)"
        return self.run_cli(["wallet", "new"], pattern=wallet_pattern)

    def init_config(self, path: str):
        endpoint = "http://127.0.0.1:8529"
        self.config_path = path
        self.run_cli(["config", "init"])
        self.run_cli(["config", "set", "rpc_endpoint", endpoint])

    def keygen(self) -> str:
        return self.run_cli(["keygen", "new"])

    def deploy_contract(self, bytecode_file: str, abi_file: str) -> str:
        contract_pattern = "Contract address: (0x[0-9a-f]+)"
        deploy_args = ["wallet", "deploy", bytecode_file]
        deploy_args += ["--abi", abi_file]
        salt = random.getrandbits(64)
        deploy_args += ["--salt", str(salt)]
        contract_address = self.run_cli(deploy_args, pattern=contract_pattern)
        return contract_address

    def call_contract(self, contract_address: str, method_name: str,arguments: list[str],
                      abi_file: str):
        message_pattern = "Message hash: (0x[0-9a-f]+)"
        call_args = ["wallet", "send-message", contract_address, method_name] + arguments
        call_args += ["--abi", abi_file]
        #call_args += ["--no-wait"] # force put transaction into the one block
        return self.run_cli(call_args, pattern=message_pattern)

# Remove directory recursively
def rmtree(f: Path):
    if f.is_file():
        f.unlink()
    else:
        for child in f.iterdir():
            rmtree(child)
        f.rmdir()

# Temporary directory scope
@contextmanager
def temp_directory(artifacts_dir: str):
    d = Path(artifacts_dir)
    try:
        d.mkdir()
        yield
    finally:
        rmtree(d)

# Compile solidity contract
def solc_compile(source: Path, out_dir: Path) -> tuple[str, str]:
    if out_dir.exists():
        # Only compilation into an empty directory is allowed,
        # otherwise we are unable to determine output file name
        raise RuntimeError("Directory must not exist")
    out_dir.mkdir()
    solc_args = ["solc", "-o", str(out_dir), "--bin", "--abi", str(source)]
    logging.info("Launching " + " ".join(solc_args))
    p = subprocess.run(solc_args, capture_output=True)
    if (p.returncode != 0):
        raise RuntimeError("Compilation failed: " + p.stderr.decode())

    # Ensure that we got only 2 expected files (.bin and .abi)
    artifact_counter = 0
    for artifact in out_dir.iterdir():
        if artifact.suffix == ".abi":
            abi_file = str(artifact)
        elif artifact.suffix == ".bin":
            bin_file = str(artifact)
        else:
            raise RuntimeError("Unexpected compiler artifact: " + str(artifact))
        artifact_counter += 1

    if artifact_counter != 2:
        raise RuntimeError("Insufficient compiler artifacts for " + str(source))
    return bin_file, abi_file

ARTIFACT_DIR = "artifacts"

# Compilation and deployment of solidity contract
class Contract:
    def __init__(self, path: str):
        self.src = Path(path)
        self.deployed = False

    def compile(self):
        # Make unique directory for contract compiler artifacts
        src_hash = hashlib.md5(str(self.src.resolve()).encode())
        out_dir = Path(ARTIFACT_DIR) / src_hash.hexdigest()
        self.bin, self.abi = solc_compile(self.src, out_dir)

    def deploy(self, cli: NilCLI):
        assert not self.deployed
        self.compile()
        self.address = cli.deploy_contract(self.bin, self.abi)
        self.deployed = True

# Deploy contracts
def deploy_contracts(cli: NilCLI, contract_map: Dict[int, Contract]):
        for _, contract in contract_map.items():
            if not contract.deployed:
                contract.deploy(cli)

# Send message to call deployed contract
def submit_transactions(cli: NilCLI, contract_map: Dict[int, Contract], transactions) -> tuple[str, Contract]:
    last_message_hash = None
    last_called_contract = None
    for transaction in transactions:
        contract_id = transaction["contractId"]
        if contract_id not in contract_map:
            raise ValueError(f"Contract id {contract_id} is not defined")

        contract = contract_map[contract_id]
        if not contract.deployed:
            raise ValueError(f"Contract id {contract_id} is not deployed")
        call_args = transaction["callArguments"] if "callArguments" in transaction else []
        last_called_contract = contract
        last_message_hash = cli.call_contract(contract.address, transaction["methodName"], call_args, contract.abi)


    return last_message_hash, last_called_contract

# extract block hash and shard id for submitted message
def get_block_hash(cli: NilCLI, message_hash: str, called_contract: str) -> tuple[str, str]:
    if message_hash is None:
            raise ValueError(f"Message hash is empty")
    receipt_str = cli.run_cli(["receipt", message_hash])
    receipt_str = re.split("Receipt data: ", receipt_str)[1]
    receipt_json = json.loads(receipt_str)

    # Recursively find receipt for a message that calls the contract execution
    def find_execution_receipt(receipt_json, contract_address):
        while True:
            if receipt_json["contractAddress"] == contract_address:
                return receipt_json
            for output_receipt in receipt_json["outputReceipts"]:
                res = find_execution_receipt(output_receipt, contract_address)
                if res is not None:
                    return res

            return None

    target_receipt = find_execution_receipt(receipt_json, called_contract)
    if target_receipt is None:
        raise RuntimeError("Receipt for the last execution message is not found")

    return str(target_receipt["shardId"]), target_receipt["blockHash"]

# extract block by hash and shard id
def extract_block(cli: NilCLI, shard_id: str, block_hash: str):
    block_str = cli.run_cli(["block", block_hash, "--shard-id", shard_id])
    block_str = re.split("Block data: ", block_str)[1]
    block_json = json.loads(block_str)
    return block_json

# Load json and validate it via schema
def validate_json(file_path: str, schema_path: str):
    with open(schema_path) as s:
        schema_json = json.load(s)
    with open(file_path) as config:
        config_json = json.load(config)
    jsonschema.validate(instance=config_json, schema=schema_json)
    return config_json


# Deploy the contract and generate transactions by calling it
def generate_block(block_config: str, cli_config_name: str, legacy_config_name):
    # Get schema file by relative path
    module_path = Path(__file__).resolve()
    schema_path = module_path.parent / "resources" / "block_schema.json"
    config_json = validate_json(block_config, str(schema_path))

    # Collect contract list
    contracts = {}
    for contract in config_json["contracts"]:
        contract_id = contract["id"]
        if contract_id in contracts:
            raise ValueError(f"Duplicated contract id: {contract_id}")
        contracts[contract_id] = Contract(contract["path"])

    # Submit transactions
    with ClusterProcess():
        cli = NilCLI(cli_config_name)
        with temp_directory(ARTIFACT_DIR):
            deploy_contracts(cli, contracts)
            last_message_hash, last_called_contract = submit_transactions(cli, contracts, config_json["transactions"])
            (shard_id, block_hash) = get_block_hash(cli, last_message_hash, last_called_contract.address)

            # Generate legacy config files with block and state if requested (to be removed)
            if legacy_config_name is not None:
                # Read contract code from the file
                with open(last_called_contract.bin) as code_file:
                    contract_code = code_file.readlines()[0].strip()

                # Write state config
                state_config = legacy_state_config(last_called_contract.address, cli.wallet_address(), contract_code)
                with open(legacy_config_name + ".state", 'w') as sf:
                    json.dump(state_config, sf)

                # Write block config
                block_json = extract_block(cli, shard_id, block_hash)
                block_config = legacy_block_config(block_json)
                with open(legacy_config_name + ".block", 'w') as bf:
                    json.dump(block_config, bf)


    print("ShardId = " + str(shard_id))
    print("BlockHash = " + block_hash)

# Generate block config in legacy format (to be removed)
def legacy_block_config(true_block):
    block_header = {}
    dumb_address = "0x0000000000000000000000000000000000000005"
    block_header["parent_hash"] = 0
    block_header["number"] = 1
    block_header["gas_limit"] = 2
    block_header["gas_used"] = 3
    block_header["coinbase"] = dumb_address
    block_header["prevrandao"] = 4
    block_header["chain_id"] = 1
    block_header["basefee"] = 55
    block_header["blob_basefee"] = 55
    block_header["timestamp"] = 5

    transaction_template = {}
    transaction_template["type"] = "MessageCall"
    transaction_template["nonce"] = 0

    block = {}
    block["previous_header"] = block_header
    block["current_header"] = block_header
    block["account_blocks"] = []
    block["transactions"] = []
    block["input_messages"] = []
    block["output_messages"] = []

    transaction_id = 0
    for msg in true_block["messages"]:
        legacy_msg = {}
        legacy_msg["src"] = msg["from"]
        legacy_msg["dst"] = msg["to"]
        legacy_msg["value"] = int(msg["value"])
        legacy_msg["transaction"] = transaction_id

        legacy_transaction = transaction_template.copy()
        legacy_transaction["id"] = transaction_id
        legacy_transaction["value"] = int(msg["value"])
        legacy_transaction["receive_address"] = msg["to"]
        legacy_transaction["sender"] = msg["from"]
        legacy_transaction["gas_price"] = int(msg["gasPrice"])
        legacy_transaction["gas"] = int(msg["gasUsed"])
        legacy_transaction["data"] = msg["data"]

        block["input_messages"].append(legacy_msg)
        block["transactions"].append(legacy_transaction)
        transaction_id += 1

    return block

# Generate account storage state config in legacy format (to be removed)
def legacy_state_config(contract_address: str, wallet_address: str, contract_code: str):
    state_config = {"accounts": []}
    contract_desc = {
        "address": contract_address.lower(),
        "balance": 0,
        "code": "0x" + contract_code
    }
    state_config["accounts"].append(contract_desc)

    wallet_desc = {
        "address": wallet_address.lower(),
        "balance": 1000000 # Some big value, not meaningful for the test
    }
    state_config["accounts"].append(wallet_desc)
    return state_config



# Write block to file
def write_block_to_file(shard_id: str, block_hash: str, block_file_name: str, cli_config_name: str):
    with ClusterProcess():
        cli = NilCLI(cli_config_name)
        block_json = extract_block(cli, shard_id, block_hash)
        with open(block_file_name, 'w') as f:
            json.dump(block_json, f)

# Initialize CLI config with endpoint, private key and new wallet address (on main shard)
def make_config(path: str):
    # remove if file already exists
    if os.path.isfile(path):
        os.remove(path)

    cli = NilCLI()
    cli.init_config(path)
    cli.keygen()
    with ClusterProcess():
        cli.new_wallet()

def parse_arguments():
    parser = argparse.ArgumentParser(prog="Nil block generator",
                                    description="Tool for contract deployment and creating transactions to generate new blocks")
    parser.add_argument("--mode", choices=["make-config", "generate-block", "write-file"],required=True, help="mode")
    parser.add_argument("--cli-config-name", help="NIL CLI extra config name describing additional wollets")
    parser.add_argument("-b", "--block-config-name", help="Block config name describing transactions")
    parser.add_argument("-o", "--block-output-name", help="Block output file name")
    parser.add_argument("--shard-id", help="Shard ID")
    parser.add_argument("--block-hash", help="Block Hash")
    parser.add_argument("-l", "--generated-legacy-configs", help="Legacy config output (to be removed)")
    parser.add_argument("-v", "--verbose", help="Enable debug logs", action="store_true")

    args = parser.parse_args()
    # Extra checks for 'make-conig' mode
    if args.mode == "make-config":
        if args.cli_config_name is None:
            raise ValueError("--cli-config-name argument must be passed for 'make-config' mode")
    # Extra checks for 'generate-blocks' mode
    if args.mode == "generate-block":
        if args.block_config_name is None:
            raise ValueError("--block-config-name argument must be passed for 'generate-blocks' mode")
    # Extra checks for 'write ile' mode
    if args.mode == "write-file":
        if (args.block_output_name is None or args.shard_id is None or args.block_hash is None):
            raise ValueError("--block-output-name, --shard-id, --block-hash arguments must be passed for 'write-file' mode")
    return args

if __name__ == "__main__":
    args = parse_arguments()
    if args.verbose:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)
    if args.mode == "make-config":
        make_config(args.cli_config_name)
    elif args.mode == "generate-block":
        generate_block(args.block_config_name, args.cli_config_name, args.generated_legacy_configs)
    elif args.mode == "write-file":
        write_block_to_file(args.shard_id, args.block_hash, args.block_output_name, args.cli_config_name)
    else:
        raise ValueError("Unknown mode")
