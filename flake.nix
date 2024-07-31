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
      url = "ssh://git@github.com/NilFoundation/nil";
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
    in
    rec {
      packages = rec {
        zkevm-framework = (pkgs.callPackage ./zkevm-framework.nix {
          src_repo = self;
          inherit crypto3 blueprint evm-assigner cluster;
        });
        debug = (pkgs.callPackage ./zkevm-framework.nix {
          src_repo = self;
          enableDebug = true;
          inherit crypto3 blueprint evm-assigner cluster;
        });
        default = zkevm-framework;
      };
      checks.default = (pkgs.callPackage ./zkevm-framework.nix {
        src_repo = self;
        enableTesting = true;
        inherit crypto3 blueprint evm-assigner cluster;
      });
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
    }
    );
}
