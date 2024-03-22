# zkEVM-framework

Framework for test verification of state of claster based on EVM

## Content

* Message generator
* EVM assigner
* Proof generator

## Dependencies

### SSZ

* Hashtree [Repository](https://github.com/prysmaticlabs/hashtree)
* intx: GMP (intx dependency) [Homepage](https://gmplib.org/), intx [Repository](https://github.com/chfast/intx)
* SSZ++ [Repository](https://github.com/OffchainLabs/sszpp)

## Nix environment

Instead of installing all dependencies manually,
you can activate [Nix](https://nixos.org/download#download-nix) environment with all installed deps:

```plain
nix develop
```

## Build

```plain
cmake -G "Ninja" -B build -DCMAKE_BUILD_TYPE=Release .
cmake --build build
```

## Test

### Configure tests

Configure with tests enabled:

```bash
cmake -B ${BUILD_DIR:-build} -DENABLE_TESTS=TRUE ...
```

When configuring with enabled tests, you can specify, which tests to enable. By default all tests are enabled.

```bash
# SSZ tests won't be enabled
cmake -B ${BUILD_DIR:-build} -DENABLE_TESTS=TRUE -DENABLE_SSZ_TESTS=FALSE ...
```

### Build tests

```bash
cmake --build ${BUILD_DIR:-build}
```

When using `Ninja` generator, you can build only tests using target `tests/all`:

```bash
cmake --build ${BUILD_DIR:-build} -t tests/all
```

### Run tests

```bash
ctest --test-dir ${BUILD_DIR:-build}/tests
```

## Build API documentation

zkEVM-framework is using Doxygen to generate API documentaion.
To build it, firts enable building documentation at configuration:

```bash
cmake -B ${BUILD_DIR:-build} -DBUILD_DOCS=TRUE ...
```

Then build documentation:

```bash
cmake -B ${BUILD_DIR:-build} -t docs
```

To see HTML documentation, open `${BUILD_DIR}/html`.

## Usage

```plain
None
```
