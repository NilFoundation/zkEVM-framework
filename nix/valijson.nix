{ repo, pkgs, stdenv, enable_debug, ... }:
stdenv.mkDerivation {
  name = "valijson";

  src = repo;

  nativeBuildInputs = [ pkgs.cmake ];

  cmakeFlags = [
    "-Dvalijson_USE_EXCEPTIONS=OFF"
  ];

  doCheck = true;
  dontStrip = enable_debug;
}
