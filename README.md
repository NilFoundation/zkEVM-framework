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

## Usage

```plain
None
```
