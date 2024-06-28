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
import subprocess
from time import sleep
from typing import Dict

# Interface for launching nil cluster RPC server
class ClusterProcess:
    def __init__(self):
        logging.info("Launching nil cluster")
        self.process = subprocess.Popen(["nil"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
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
    def __init__(self, config_path: str):
        self.config_path = config_path

    def run_cli(self, args: list[str], pattern=None) -> str:
        full_arg_list = ["nil_cli", "-c", self.config_path] + args
        logging.info("Launching " + " ".join(full_arg_list))
        p = subprocess.run(full_arg_list, capture_output=True)
        logging.debug(p.stderr)
        if p.returncode != 0:
            raise RuntimeError(p.stderr)
        output = p.stderr.decode()
        if pattern is None:
            return output
        a = re.search(pattern, output)
        if a is None:
            raise RuntimeError(p.stderr)
        return a.group(1)

    def keygen(self) -> str:
        key_pattern = "key: ([0-9a-f]+)"
        return self.run_cli(["keygen", "--new"], pattern=key_pattern)

    def new_wallet(self) -> str:
        wallet_pattern = "New wallet address: (0x[0-9a-fA-F]+)"
        return self.run_cli(["wallet", "new"], pattern=wallet_pattern)

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
        call_args = ["wallet", "send-message", contract_address, method_name] + arguments
        call_args += ["--abi", abi_file]
        self.run_cli(call_args)

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

# Deploy contracts and send requested messages to them
def submit_transactions(cli: NilCLI, contract_map: Dict[int, Contract], transactions):
    for transaction in transactions:
        contract_id = transaction["contractId"]
        if contract_id not in contract_map:
            raise ValueError(f"Contract id {contract_id} is not defined")

        contract = contract_map[contract_id]
        if not contract.deployed:
            contract.deploy(cli)
        call_args = transaction["callArguments"] if "callArguments" in transaction else []
        cli.call_contract(contract.address, transaction["methodName"], call_args, contract.abi)

# Load json and validate it via schema
def validate_json(file_path: str, schema_path: str):
    with open(schema_path) as s:
        schema_json = json.load(s)
    with open(file_path) as config:
        config_json = json.load(config)
    jsonschema.validate(instance=config_json, schema=schema_json)
    return config_json


# Deploy the contract and generate transactions by calling it
def block_generator(cli_config: str, block_config: str):
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
        cli = NilCLI(cli_config)
        with temp_directory(ARTIFACT_DIR):
            submit_transactions(cli, contracts, config_json["transactions"])

# Initialize CLI config with endpoint, private key and new wallet address (on main shard)
def make_config(path: str):
    with open(path, "w") as config_file:
        config_file.write("rpc_endpoint: \"http://127.0.0.1:8529\"\n")
    cli = NilCLI(path)
    private_key = cli.keygen()
    with open(path, "a") as config_file:
        config_file.write(f"private_key: {private_key}\n")
    with ClusterProcess():
        wallet_address = cli.new_wallet()
        with open(path, "a") as config_file:
            config_file.write(f"address: {wallet_address}\n")

def parse_arguments():
    parser = argparse.ArgumentParser(prog="Nil block generator",
                                    description="Tool for contract deployment and creating transactions to generate new blocks")
    parser.add_argument("--mode", choices=["make-config", "generate-blocks"],required=True, help="mode")
    parser.add_argument("-c", "--cli-config-name", default="config.yaml", help="CLI config file name")
    parser.add_argument("-b", "--block-config-name", help="Block config name describing transactions")
    parser.add_argument("-v", "--verbose", help="Enable debug logs", action="store_true")

    args = parser.parse_args()
    # Extra checks for 'generate-blocks' mode
    if args.mode == "generate-blocks":
        if args.block_config_name is None:
            raise ValueError("--block-config-name argument must be passed for 'generate-blocks' mode")
        config_path = Path(args.block_config_name)
        if not config_path.exists():
            raise FileNotFoundError(f"Block configuration file {str(config_path)} does not exist")
    return args

if __name__ == "__main__":
    args = parse_arguments()
    if args.verbose:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)
    if args.mode == "make-config":
        make_config(args.cli_config_name)
    else:
        block_generator(args.cli_config_name, args.block_config_name)
