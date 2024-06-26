#!/usr/bin/env python3

import argparse
import subprocess
import re
import random
from pathlib import Path
from time import sleep
import logging

# Interface for launching nil cluster RPS server
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

    def run_cli(self, args: list[str], pattern=None):
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

    def keygen(self):
        key_pattern = "key: ([0-9a-f]+)"
        return self.run_cli(["keygen", "--new"], pattern=key_pattern)

    def new_wallet(self):
        wallet_pattern = "New wallet address: (0x[0-9a-fA-F]+)"
        return self.run_cli(["wallet", "new"], pattern=wallet_pattern)

    def deploy_contract(self, bytecode_file: str, abi_file: str):
        contract_pattern = "Contract address: (0x[0-9a-f]+)"
        deploy_args = ["wallet", "deploy", bytecode_file]
        deploy_args += ["--abi", str(abi_file)]
        salt = random.getrandbits(64)
        deploy_args += ["--salt", str(salt)]
        contract_address = self.run_cli(deploy_args, pattern=contract_pattern)
        return contract_address

    def call_contract(self, contract_address: str, method_name: str, abi_file: str):
        call_args = ["wallet", "send-message", contract_address, method_name]
        call_args += ["--abi", abi_file]
        self.run_cli(call_args)

# Deploy the contract and generate transactions by calling it
def naive_generator(config_file: str, bytecode_file: str, method_name: str):
    with ClusterProcess():
        abi_file = Path(bytecode_file).with_suffix(".abi")
        if not abi_file.exists():
            raise FileNotFoundError(f"ABI file for {bytecode_file} was not found")
        cli = NilCLI(config_file)
        contract_addr = cli.deploy_contract(bytecode_file, str(abi_file))
        # Generate bunch of transactions by calling the contract
        for _ in range(10):
            cli.call_contract(contract_addr, method_name, str(abi_file))

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
    parser.add_argument("-c", "--config-name", default="config.yaml", help="CLI config file name")
    parser.add_argument("-b", "--contract-binary", help="compiled contract")
    parser.add_argument("-f", "--method-name", help="method of contract to call")
    parser.add_argument("-v", "--verbose", help="Enable debug logs", action="store_true")

    args = parser.parse_args()
    # Extra checks for 'generate-blocks' mode
    if args.mode == "generate-blocks":
        if args.contract_binary is None:
            raise ValueError("--contract-binary argument must be passed for 'generate-blocks' mode")
        if args.method_name is None:
            raise ValueError("--method-name argument must be passed for 'generate-blocks' mode")
        binary_path = Path(args.contract_binary)
        if not binary_path.exists():
            raise FileNotFoundError(f"Contract binary {str(binary_path)} does not exist")
    return args

args = parse_arguments()
if args.verbose:
    logging.basicConfig(level=logging.DEBUG)
else:
    logging.basicConfig(level=logging.INFO)
if args.mode == "make-config":
    make_config(args.config_name)
else:
    naive_generator(args.config_name, args.contract_binary, args.method_name)
