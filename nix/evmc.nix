{ repo, pkgs, stdenv, enable_debug, ... }:
stdenv.mkDerivation {
  name = "evmc";

  src = repo;

  nativeBuildInputs = [ pkgs.cmake ];

  cmakeFlags = [
    "-DEVMC_TOOLS=FALSE"
  ];

  doCheck = false;
  dontStrip = enable_debug;
}
