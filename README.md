# RISC-V Emulator

An emulator of ISA RISC-V (RV32IM) with RAM and Cache.

## About

This project contains 3 major parts:

* **RAM:** keeps commands and data
* **Cache (LRU and bit-pLRU policies):** is the same for both commands and data (Von Neumann architecture)
* **CPU:** executes commands

## Getting started

**Prerequisites:**

* CMake 3.12+
* `C++23` compatible compiler

```bash
mkdir build
cmake -B build -S .
cd build
cmake --build . --target risc-v-emulator
```

### CLI Usage

```bash
./risc-v-emulator -i <input_file> -o <address> <size>
```

* `-i <input_file>` - input file in binary format. The format of file is the following:
  * `pc` register (4 bytes)
  * 32 main registers (32 x 4 bytes)
  * RAM segments: `[address (4 bytes)] [size (4 bytes)] [raw_data]`
* `-o <output_file> <address> <size>` - dumps the part of RAM beginning on `address` and size `size` to `output_file`. Not necessary.

**Endianess:** little-endian

**Important:** `a1` register is used as an `ra` register. Emulator finishes simulation when `pc` reaches value stored in `ra`.

### Examples

The `examples/` directory contains several pre-compiled programs and their assembly sources to demonstrate the emulator's capabilities:

* **`fill`**: Calculates squares for numbers 0–99 and writes the results to RAM starting at address `0x2000`. Perfect for verifying basic ALU and Store operations.
* **`sort`**: Initializes an array of integers at `0x2000` and then sorts it using the **Bubble Sort** algorithm. This test validates complex control flow, nested loops, and memory load/store consistency.
* **`cache`**: A specialized test case designed to verify cache policy efficiency. It is tuned to achieve specific hit rates: **96% for instructions** and **75% for data**, providing a benchmark for the LRU and bit-pLRU implementation.

### How to generate your own .bins
