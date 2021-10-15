## Advent of Code solutions

This is my attempt at solving and optimizing [Advent of Code](https://adventofcode.com/) puzzles in C++.

## Solutions per year

For timings and overview of the used algorithms visit the root directory of each year.

[2020](https://github.com/divisorr/adventofcode/tree/master/2020)

## Algorithms runtime

All timings are in microseconds.

    Year    Ryzen 3600        Pi 400
    ================================
    2020     403949 us    2899713 us

## Requirements

Compiler support for C++20 features such as std\::popcount, std\::countr_zero is required.
I used these compilers: GCC 10.2, MSVC v143, Clang 12.0.0

## Usage

- The solutions should work with any input, provided it is unmodified and with LF endings.
- If you are using Visual Studio with the provided .sln file compile and run.
- If you are using CMake place the compiled executable in the same directory as the input files.
