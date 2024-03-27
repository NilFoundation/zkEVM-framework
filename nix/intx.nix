{ repo, pkgs, stdenv, enable_debug, ... }:
stdenv.mkDerivation {
  name = "intx";

  src = repo;

  nativeBuildInputs = [ pkgs.cmake ];

  cmakeFlags = [
    "-DINTX_TESTING=OFF"
  ];

  doCheck = true;
  dontStrip = enable_debug;
}
