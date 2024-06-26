{
  description = "Nix flake for zkEVM framework";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
    nix-3rdparty = {
      url = "github:NilFoundation/nix-3rdparty";
      inputs = {
        nixpkgs.follows = "nixpkgs";
        flake-utils.follows = "flake-utils";
      };
    };
    nil-evm-assigner = {
      type = "github";
      owner = "NilFoundation";
      repo = "evm-assigner";
      inputs = {
        nixpkgs.follows = "nixpkgs";
        flake-utils.follows = "flake-utils";
        nix-3rdparty.follows = "nix-3rdparty";
        nil_crypto3.follows = "nil-crypto3";
        nil_zkllvm_blueprint.follows = "nil-zkllvm-blueprint";
      };
    };
    nil-cluster = {
      type = "github";
      owner = "NilFoundation";
      repo = "nil";
      inputs = {
        nixpkgs.follows = "nixpkgs";
        flake-utils.follows = "flake-utils";
      };
    };
    nil-crypto3 = {
      url = "https://github.com/NilFoundation/crypto3";
      type = "git";
      submodules = true;
      inputs = {
        nixpkgs.follows = "nixpkgs";
      };
    };
    nil-zkllvm-blueprint = {
      url = "https://github.com/NilFoundation/zkllvm-blueprint";
      type = "git";
      submodules = true;
      inputs = {
        nixpkgs.follows = "nixpkgs";
        flake-utils.follows = "flake-utils";
        nil_crypto3.follows = "nil-crypto3";
      };
    };
  };

  outputs =
    { self
    , nixpkgs
    , flake-utils
    , nix-3rdparty
    , nil-evm-assigner
    , nil-crypto3
    , nil-zkllvm-blueprint
    , nil-cluster
    }:
    flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = import nixpkgs {
        overlays = [ nix-3rdparty.overlays.${system}.default ];
        inherit system;
      };
      evm_assigner_pkgs = nil-evm-assigner.packages.${system};
      evm_assigner = { enableDebug ? false }:
        if enableDebug then evm_assigner_pkgs.debug else evm_assigner_pkgs.default;

      crypto3 = nil-crypto3.packages.${system}.default;
      blueprint = nil-zkllvm-blueprint.packages.${system}.default;
      cluster = nil-cluster.packages.${system}.default;

      # Default env will bring us GCC 13 as default compiler
      stdenv = pkgs.stdenv;

      defaultNativeBuildInputs = [
        pkgs.cmake
        pkgs.ninja
      ];

      defaultBuildInputs = { enableDebug ? false }: [
        # Default nixpkgs packages
        pkgs.boost
        pkgs.python3
        pkgs.solc
        pkgs.valijson
        # Packages from nix-3rdparty
        (pkgs.sszpp.override { inherit enableDebug; })
        (pkgs.evmc.override { inherit enableDebug; })
        # Repo dependencies
        (evm_assigner { inherit enableDebug; })
        crypto3
        blueprint
        cluster
      ];

      defaultCheckInputs = [
        pkgs.gtest
      ];

      defaultDevTools = [
        pkgs.doxygen
        pkgs.clang_17 # clang-format and clang-tidy
        pkgs.go_1_22
        pkgs.gotools
        pkgs.go-tools
        pkgs.gopls
        pkgs.golangci-lint
        pkgs.gofumpt
        pkgs.gci
      ];

      releaseBuild = stdenv.mkDerivation {
        name = "zkEVM-framework";

        nativeBuildInputs = defaultNativeBuildInputs;

        buildInputs = defaultBuildInputs { };

        src = self;

        cmakeBuildType = "Release";

        doCheck = false;
      };

      debugBuild = releaseBuild.overrideAttrs (finalAttrs: previousAttrs: {
        name = previousAttrs.name + "-debug";

        buildInputs = defaultBuildInputs { enableDebug = true; };

        cmakeBuildType = "Debug";

        dontStrip = true;
      });

      testBuild = releaseBuild.overrideAttrs (finalAttrs: previousAttrs: {
        name = previousAttrs.name + "-tests";

        cmakeFlags = [ "-DENABLE_TESTS=TRUE" ];

        doCheck = true;

        checkInputs = defaultCheckInputs;

        GTEST_OUTPUT = "xml:${placeholder "out"}/test-reports/";

        dontInstall = true;
      });

      makeDevShell = { enableDebug }: pkgs.mkShell {
        nativeBuildInputs = defaultNativeBuildInputs
          ++ defaultBuildInputs { inherit enableDebug; }
          ++ defaultCheckInputs
          ++ defaultDevTools;

        shellHook = ''
          echo "zkEVM-framework ${if enableDebug then "debug" else "release"} dev environment activated"
        '';
      };
    in
    rec {
      packages = rec {
        default = release;
        release = releaseBuild;
        debug = debugBuild;
      };
      checks.default = testBuild;
      apps = {
        assigner = {
          type = "app";
          program = "${packages.default}/bin/assigner";
        };
        block_gen = {
          type = "app";
          program = "${packages.default}/bin/block_gen";
        };
      };
      devShells = rec {
        default = debug;
        release = makeDevShell { enableDebug = false; };
        debug = makeDevShell { enableDebug = true; };
      };
    }
    );
}
