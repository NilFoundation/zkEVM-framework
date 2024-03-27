{
  description = "Nix flake for zkEVM framework";

  inputs = {
    nixpkgs = { url = "github:NixOS/nixpkgs"; };
    intx = { url = "github:chfast/intx"; flake = false; };
    hashtree = { url = "github:prysmaticlabs/hashtree"; flake = false; };
    sszpp = { url = "github:OffchainLabs/sszpp"; flake = false; };
  };

  outputs = { self, nixpkgs, ... } @ repos:
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
        };

      makeReleaseBuild = { pkgs }:
        let
          deps = allDependencies { enable_debug = false; inherit repos pkgs; };
        in
        pkgs.gcc13Stdenv.mkDerivation {
          name = "zkEVM-framework";

          buildInputs = with pkgs; [
            cmake
            ninja
            deps.sszpp
          ];

          src = self;

          doCheck = false;
        };

      makeTests = { pkgs }:
        let
          deps = allDependencies { enable_debug = false; inherit repos pkgs; };
        in
        pkgs.gcc13Stdenv.mkDerivation {
          name = "zkEVM-framework-tests";

          src = self;

          buildInputs = with pkgs; [
            cmake
            ninja
            deps.sszpp
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
        in
        pkgs.mkShell {
          buildInputs = with pkgs; [
            cmake
            ninja
            gtest
            doxygen
            clang_17
            deps.sszpp
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
