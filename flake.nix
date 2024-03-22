{
  description = "zkEVM framework environment";

  inputs = {
    nixpkgs = { url = "github:NixOS/nixpkgs"; };
  };

  outputs = { self, nixpkgs, ... }:
    let
      supportedSystems = [
        "x86_64-linux"
      ];

      makeShell = { system }:
        let
          pkgs = import nixpkgs {
            inherit system;
          };
          lib = pkgs.lib;
          stdenv = pkgs.gcc13Stdenv;

          hashtree = (pkgs.callPackage ./nix/hashtree.nix) { inherit pkgs lib stdenv; };
          intx = (pkgs.callPackage ./nix/intx.nix) { inherit pkgs lib stdenv; };
          sszpp = (pkgs.callPackage ./nix/sszpp.nix) { inherit pkgs lib stdenv hashtree intx; };

        in
        pkgs.mkShell {
          nativeBuildInputs = with pkgs; [ cmake ninja gtest doxygen ];
          buildInputs = [ sszpp ];
        };
    in
    {
      devShells = nixpkgs.lib.genAttrs
        supportedSystems
        (system: { default = makeShell { inherit system; }; });
    };
}
