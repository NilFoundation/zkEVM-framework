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
        nil-crypto3.follows = "nil-crypto3";
        nil-zkllvm-blueprint.follows = "nil-zkllvm-blueprint";
      };
    };
    nil-cluster = {
      type = "git";  # 'git' is required here, for 'github' we cannot compute cluster version via git history
      url = "https://github.com/NilFoundation/nil";
      inputs = {
        nixpkgs.follows = "nixpkgs";
        flake-utils.follows = "flake-utils";
      };
    };
    nil-crypto3 = {
      type = "github";
      owner = "NilFoundation";
      repo = "crypto3";
      rev = "3bd5b8df2091274abaa28fd86b9e3e89d661b95a";
      inputs = {
        nixpkgs.follows = "nixpkgs";
        flake-utils.follows = "flake-utils";
        nix-3rdparty.follows = "nix-3rdparty";
      };
    };
    nil-zkllvm-blueprint = {
      url = "https://github.com/NilFoundation/zkllvm-blueprint";
      type = "git";
      submodules = true;
      rev = "73d6a40e39b6b6fc7b1c84441e62337206dc0815";
      inputs = {
        nixpkgs.follows = "nixpkgs";
        flake-utils.follows = "flake-utils";
        nil-crypto3.follows = "nil-crypto3";
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
      evm-assigner-pkgs = nil-evm-assigner.packages.${system};
      evm-assigner = { enableDebug ? false }:
        if enableDebug then evm-assigner-pkgs.debug else evm-assigner-pkgs.default;

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
        pkgs.python3
        pkgs.python312Packages.jsonschema
        pkgs.solc
        pkgs.valijson
        # Packages from nix-3rdparty
        (pkgs.intx.override { inherit enableDebug; })
        (pkgs.sszpp.override { inherit enableDebug; })
        (pkgs.evmc.override { inherit enableDebug; })
        # Repo dependencies
        (evm-assigner { inherit enableDebug; })
        crypto3
        blueprint
        # Blueprint will propagate Boost library.
        # We don't include it here explicitly to reuse the same version.
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

         checkPhase = ''
          ctest
          ninja executables_tests
        '';

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
