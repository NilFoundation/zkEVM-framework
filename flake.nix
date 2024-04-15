{
  description = "Nix flake for zkEVM framework";

  inputs = {
    nixpkgs = { url = "github:NixOS/nixpkgs/nixos-23.11"; };
    nil_evm_assigner = {
      type = "github";
      owner = "NilFoundation";
      repo = "evm-assigner";
    };
    nil_crypto3 = {
      url = "https://github.com/NilFoundation/crypto3";
      type = "git";
      submodules = true;
    };
    # TODO: remove revision after merge of this PR in zkllvm-blueprint:
    # https://github.com/NilFoundation/zkllvm-blueprint/pull/363
    nil_zkllvm_blueprint = {
      url = "https://github.com/NilFoundation/zkllvm-blueprint";
      type = "git";
      rev = "01964c21ab68e0cd77e08c0a9225f220a5b6bf2d";
      submodules = true;
    };
    intx = { url = "github:chfast/intx"; flake = false; };
    hashtree = { url = "github:prysmaticlabs/hashtree"; flake = false; };
    sszpp = { url = "github:OffchainLabs/sszpp"; flake = false; };
    evmc = { url = "github:ethereum/evmc"; flake = false; };
  };

  outputs = { self, nixpkgs, nil_evm_assigner, nil_crypto3, nil_zkllvm_blueprint, ... } @ repos:
    let
      supportedSystems = [
        "x86_64-linux"
      ];

      # For all supported systems call `f` providing system pkgs
      forAllSystems = f: nixpkgs.lib.genAttrs supportedSystems (system: f {
        pkgs = import nixpkgs {
          inherit system;
        };
      });

      allDependencies = { pkgs, repos, enable_debug }:
        let
          lib = pkgs.lib;
          stdenv = pkgs.gcc13Stdenv;
        in
        rec {
          hashtree = (pkgs.callPackage ./nix/hashtree.nix) {
            repo = repos.hashtree;
            inherit pkgs lib stdenv enable_debug;
          };

          intx = (pkgs.callPackage ./nix/intx.nix) {
            repo = repos.intx;
            inherit pkgs lib stdenv enable_debug;
          };

          sszpp = (pkgs.callPackage ./nix/sszpp.nix) {
            repo = repos.sszpp;
            inherit pkgs lib enable_debug stdenv hashtree intx;
          };

          evmc = (pkgs.callPackage ./nix/evmc.nix) {
            repo = repos.evmc;
            inherit pkgs lib stdenv enable_debug;
          };
        };

      makeReleaseBuild = { pkgs }:
        let
          deps = allDependencies { enable_debug = false; inherit repos pkgs; };
          evm_assigner = nil_evm_assigner.packages.${pkgs.system}.default;
          crypto3 = nil_crypto3.packages.${pkgs.system}.default;
          blueprint = nil_zkllvm_blueprint.packages.${pkgs.system}.default;
        in
        pkgs.gcc13Stdenv.mkDerivation {
          name = "zkEVM-framework";

          buildInputs = with pkgs; [
            cmake
            ninja
            boost
            deps.sszpp
            evm_assigner
            crypto3
            blueprint
          ];

          src = self;

          doCheck = false;
        };

      makeTests = { pkgs }:
        let
          deps = allDependencies { enable_debug = false; inherit repos pkgs; };
          evm_assigner = nil_evm_assigner.packages.${pkgs.system}.default;
          crypto3 = nil_crypto3.packages.${pkgs.system}.default;
          blueprint = nil_zkllvm_blueprint.packages.${pkgs.system}.default;
        in
        pkgs.gcc13Stdenv.mkDerivation {
          name = "zkEVM-framework-tests";

          src = self;

          buildInputs = with pkgs; [
            cmake
            ninja
            boost
            deps.sszpp
            evm_assigner
            crypto3
            blueprint
          ];

          cmakeFlags = [ "-DENABLE_TESTS=TRUE" ];

          doCheck = true;
          checkInputs = with pkgs; [
            gtest
          ];

          GTEST_OUTPUT = "xml:${placeholder "out"}/test-reports/";

          dontInstall = true;
        };

      makeDevShell = { pkgs }:
        let
          deps = allDependencies { enable_debug = true; inherit repos pkgs; };
          evm_assigner = nil_evm_assigner.packages.${pkgs.system}.default;
          crypto3 = nil_crypto3.packages.${pkgs.system}.default;
          blueprint = nil_zkllvm_blueprint.packages.${pkgs.system}.default;

        in
        pkgs.mkShell {
          buildInputs = with pkgs; [
            cmake
            ninja
            boost
            gtest
            doxygen
            clang_17
            deps.sszpp
            evm_assigner
            crypto3
            blueprint
          ];

          shellHook = ''
            echo "zkEVM dev environment activated"
          '';
        };
    in
    {
      packages = forAllSystems ({ pkgs }: { default = makeReleaseBuild { inherit pkgs; }; });
      checks = forAllSystems ({ pkgs }: { default = makeTests { inherit pkgs; }; });
      devShells = forAllSystems ({ pkgs }: { default = makeDevShell { inherit pkgs; }; });
    };
}
