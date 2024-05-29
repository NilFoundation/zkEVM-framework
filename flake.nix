{
  description = "Nix flake for zkEVM framework";

  inputs = {
    nixpkgs = { url = "github:NixOS/nixpkgs"; };
    nix-3rdparty = {
      url = "github:NilFoundation/nix-3rdparty";
      inputs = {
        nixpkgs.follows = "nixpkgs";
      };
    };
    nil_evm_assigner = {
      type = "github";
      owner = "NilFoundation";
      repo = "evm-assigner";
      inputs = {
        nixpkgs.follows = "nixpkgs";
        nil_crypto3.follows = "nil_crypto3";
        nil_zkllvm_blueprint.follows = "nil_zkllvm_blueprint";
      };
    };
    nil_crypto3 = {
      url = "https://github.com/NilFoundation/crypto3";
      type = "git";
      submodules = true;
      inputs = {
        nixpkgs.follows = "nixpkgs";
      };
    };
    nil_zkllvm_blueprint = {
      url = "https://github.com/NilFoundation/zkllvm-blueprint";
      type = "git";
      submodules = true;
      inputs = {
        nixpkgs.follows = "nixpkgs";
        nil_crypto3.follows = "nil_crypto3";
      };
    };
  };

  outputs = { self, nixpkgs, nix-3rdparty, nil_evm_assigner, nil_crypto3, nil_zkllvm_blueprint, ... } @ repos:
    let
      supportedSystems = [
        "x86_64-linux"
      ];

      # For all supported systems call `f` providing system pkgs
      forAllSystems = f: nixpkgs.lib.genAttrs supportedSystems (system: f {
        pkgs = import nixpkgs {
          overlays = [ nix-3rdparty.overlays.${system}.default ];
          inherit system;
        };
      });

      makeReleaseBuild = { pkgs }:
        let
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
            sszpp
            evmc
            valijson
            evm_assigner
            crypto3
            blueprint
          ];

          src = self;

          doCheck = false;
        };

      makeTests = { pkgs }:
        let
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
            sszpp
            evmc
            valijson
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
          evm_assigner = nil_evm_assigner.packages.${pkgs.system}.default;
          crypto3 = nil_crypto3.packages.${pkgs.system}.default;
          blueprint = nil_zkllvm_blueprint.packages.${pkgs.system}.default;
        in
        pkgs.mkShell {
          nativeBuildInputs = with pkgs; [
            gcc13
            cmake
            ninja
            boost
            gtest
            doxygen
            clang_17
            sszpp
            evmc
            valijson
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
      apps = forAllSystems ({ pkgs }: {
        assigner = {
              type = "app";
              program = "${self.packages.${pkgs.system}.default}/bin/assigner";
        };
        block_gen = {
              type = "app";
              program = "${self.packages.${pkgs.system}.default}/bin/block_gen";
        };
      });
      devShells = forAllSystems ({ pkgs }: { default = makeDevShell { inherit pkgs; }; });
    };
}
