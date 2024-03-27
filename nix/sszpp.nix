{ repo, pkgs, stdenv, enable_debug, intx, hashtree, ... }:
stdenv.mkDerivation {
  name = "sszpp";

  src = repo;

  env.NIX_CFLAGS_COMPILE = "-Wno-error=format-security";

  nativeBuildInputs = [
    pkgs.cmake
  ];

  cmakeFlags = [
    "-DCMAKE_CXX_STANDARD=23"
  ];

  # Error in definition of this function leads to multiple definition error on linking
  patches = [
    ./patches/sszpp-merkleize-fix.diff
  ];

  doCheck = true;
  dontStrip = enable_debug;

  propagatedBuildInputs = [
    intx
    hashtree
  ];
}
