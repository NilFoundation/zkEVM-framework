{ pkgs
, lib
, stdenv
, src_repo
, cmake
, ninja
, gtest
, solc
, doxygen
, clang_17
, go_1_22
, gotools
, go-tools
, gopls
, golangci-lint
, gofumpt
, gci
, python3
, python312Packages
, valijson
, intx
, sszpp
, evmc
, evm-assigner
, crypto3
, blueprint
, cluster
, enableTesting ? false
, enableDebug ? false
}:

stdenv.mkDerivation {
  name = "zkEVM-framework";

  nativeBuildInputs = [
    cmake
    ninja
    solc
    python3
    doxygen
    clang_17 # clang-format and clang-tidy
    go_1_22
    gotools
    go-tools
    gopls
    golangci-lint
    gofumpt
    gci
  ];

  buildInputs = [
    gtest
    python312Packages.jsonschema
    valijson
    crypto3
    blueprint
    cluster
    (intx.override { inherit enableDebug; })
    (sszpp.override { inherit enableDebug; })
    (evmc.override { inherit enableDebug; })
    (evm-assigner { inherit enableDebug; })
  ];

  src = src_repo;

  cmakeBuildType = if enableDebug then "Debug" else "Release";

  dontStrip = enableDebug;

  cmakeFlags = if enableTesting then [ "-DENABLE_TESTS=TRUE" ] else [ ];

  doCheck = enableTesting;

  checkPhase = ''
    ctest
    ninja executables_tests
  '';

  GTEST_OUTPUT = "xml:${placeholder "out"}/test-reports/";

  shellHook = ''
    echo "zkEVM-framework ${if enableDebug then "debug" else "release"} dev environment activated"
  '';
}
