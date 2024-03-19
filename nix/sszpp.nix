{ pkgs, stdenv, intx, hashtree, ... }:
stdenv.mkDerivation {
  name = "sszpp";

  src = pkgs.fetchFromGitHub {
    owner = "OffchainLabs";
    repo = "sszpp";
    rev = "ec97d8976d7f0a78eccbf59cab59eae4cf508ca9";
    hash = "sha256-310yiR02zAQ9v6TIdOYDuS7lVQJRhAeIEzaeVEGnMDA=";
  };

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

  propagatedBuildInputs = [
    intx
    hashtree
  ];
}
