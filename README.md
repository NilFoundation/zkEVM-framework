# zkEVM-framework
Framework for test verification of state of claster based on EVM
## Content
- Message generator
- EVM assigner
- Proof generator
## Dependencies

- 1. SSZ
-- 1.1. Hashtree [Repository](https://github.com/prysmaticlabs/hashtree)
-- 1.2. intx
--- 1.2.1. GMP (intx dependency) [Homepage](https://gmplib.org/)
--- 1.2.2. intx [Repository](https://github.com/chfast/intx)
-- 1.3. SSZ++ [Repository](https://github.com/OffchainLabs/sszpp)

## Build
```
cmake -G "Ninja" -B build -DCMAKE_BUILD_TYPE=Release .
cmake --build build
```
## Test
## Usage
```
None
```
