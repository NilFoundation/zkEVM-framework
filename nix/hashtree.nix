{ pkgs, lib, stdenv, ... }:
stdenv.mkDerivation {
  name = "hashtree";

  src = pkgs.fetchFromGitHub {
    owner = "prysmaticlabs";
    repo = "hashtree";
    rev = "289330e9ffafec9ca64bbbb2a30cd3bbce4a00dc";
    hash = "sha256-v+aUS7WThaxD+QihFGVPhAkn6NhJU8Nsa4Mww3zJp6g=";
  };

  outputs = [ "out" ];

  env.NIX_CFLAGS_COMPILE = lib.optionalString stdenv.cc.isClang "-no-integrated-as -std=c2x";

  enableParallelBuilding = true;

  makeFlags = [ "DESTDIR=$(out)" "PREFIX=" ];
}
