# RISC-V Emulator

An emulator of ISA RISC-V (RV32I and RV32M) with RAM and Cache.

## About

This project contains 3 major parts:
- **RAM:** keeps commands and data
- **Cache (LRU and bit-pLRU policies):** is the same for both commands and data (fon Naeman architecture)
- **CPU:** executes commands

## Getting started

```
mkdir build
cmake -B build -S .
cd build
cmake --build . --target risc-v-emulator
```

### CMD options
- **-i** - input file in binary format. The format of file is the following:
- **-o**