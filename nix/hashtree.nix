{ repo, pkgs, lib, stdenv, enable_debug, ... }:
stdenv.mkDerivation {
  name = "hashtree";

  src = repo;

  outputs = [ "out" ];

  env.NIX_CFLAGS_COMPILE = lib.optionalString stdenv.cc.isClang "-no-integrated-as -std=c2x";

  enableParallelBuilding = true;

  # Without this GNU linker 2.39+ will produce a warning:
  # ld: warning: sha256_shani.o: missing .note.GNU-stack section implies executable stack
  # ld: NOTE: This behaviour is deprecated and will be removed in a future version of the linker
  patches = [
    ./patches/hashtree-execstack-fix.diff
  ];

  makeFlags = [ "DESTDIR=$(out)" "PREFIX=" ];

  doCheck = true;
  dontStrip = enable_debug;
}
