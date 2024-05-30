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
        nil_crypto3.follows = "nil-crypto3";
        nil_zkllvm_blueprint.follows = "nil-zkllvm-blueprint";
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
    }:
    flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = import nixpkgs {
        overlays = [ nix-3rdparty.overlays.${system}.default ];
        inherit system;
      };
      evm_assigner = nil-evm-assigner.packages.${system}.default;
      crypto3 = nil-crypto3.packages.${system}.default;
      blueprint = nil-zkllvm-blueprint.packages.${system}.default;

      # Default env will bring us GCC 13 as default compiler
      stdenv = pkgs.stdenv;

      defaultNativeBuildInputs = [
        pkgs.cmake
        pkgs.ninja
      ];

      defaultBuildInputs = [
        # Default nixpkgs packages
        pkgs.boost
        pkgs.valijson
        # Packages from nix-3rdparty
        pkgs.sszpp
        pkgs.evmc
        # Repo dependencies
        evm_assigner
        crypto3
        blueprint
      ];

      defaultCheckInputs = [
        pkgs.gtest
      ];

      defaultDevTools = [
        pkgs.doxygen
        pkgs.clang_17 # clang-format and clang-tidy
      ];

      releaseBuild = stdenv.mkDerivation {
        name = "zkEVM-framework";

        nativeBuildInputs = defaultNativeBuildInputs;

        buildInputs = defaultBuildInputs;

        src = self;

        cmakeBuildType = "Release";

        doCheck = false;
      };

      # TODO: we need to propagate debug mode to dependencies here:
      # Tracking issue: https://github.com/NilFoundation/zkEVM-framework/issues/58
      debugBuild = releaseBuild.overrideAttrs (finalAttrs: previousAttrs: {
        name = previousAttrs.name + "-debug";

        cmakeBuildType = "Debug";
      });

      testBuild = releaseBuild.overrideAttrs (finalAttrs: previousAttrs: {
        name = previousAttrs.name + "-tests";

        cmakeFlags = [ "-DENABLE_TESTS=TRUE" ];

        doCheck = true;

        checkInputs = defaultCheckInputs;

        GTEST_OUTPUT = "xml:${placeholder "out"}/test-reports/";

        dontInstall = true;
      });

      makeDevShell = pkgs.mkShell {
        nativeBuildInputs = defaultNativeBuildInputs
          ++ defaultBuildInputs
          ++ defaultCheckInputs
          ++ defaultDevTools;

        shellHook = ''
          echo "zkEVM-framework dev environment activated"
        '';
      };
    in
    {
      packages = {
        default = releaseBuild;
        debug = debugBuild;
      };
      checks.default = testBuild;
      apps = {
        assigner = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/assigner";
        };
        block_gen = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/block_gen";
        };
      };
      devShells.default = makeDevShell;
    }
    );
}
