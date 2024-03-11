{ pkgs, stdenv, ... }:
stdenv.mkDerivation {
  name = "intx";

  src = pkgs.fetchFromGitHub {
    owner = "chfast";
    repo = "intx";
    rev = "e0732242e36b3bb08cc8cc23445b2bee7c28b9d0";
    hash = "sha256-XNpSO++FuG27Bgf5My9eJ/zrQlsWcGajfq4Y2UI2Kpk=";
  };

  nativeBuildInputs = [ pkgs.cmake ];

  cmakeFlags = [
    "-DINTX_TESTING=OFF"
  ];
}
