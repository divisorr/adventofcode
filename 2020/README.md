## Solutions for [Advent of Code 2020](https://adventofcode.com/2020/) in C++

## Timings

           Ryzen 3600      Pi 400
    =============================
    Day 01       5 us       13 us
    Day 02      22 us       57 us
    Day 03       5 us        6 us
    Day 04      52 us      114 us
    Day 05       3 us        8 us
    Day 06      22 us       52 us
    Day 07     493 us     1612 us
    Day 08       8 us       24 us
    Day 09      76 us      154 us
    Day 10       1 us        2 us
    Day 11    4895 us    20760 us
    Day 12       9 us       21 us
    Day 13       1 us        2 us
    Day 14    1537 us    15401 us
    Day 15  207638 us  1580585 us
    Day 16      64 us      159 us
    Day 17     500 us     2682 us
    Day 18      86 us      206 us
    Day 19      32 us      100 us
    Day 20      53 us      196 us
    Day 21     388 us     1037 us
    Day 22   36319 us    93306 us
    Day 23  148418 us  1175523 us
    Day 24    3088 us     6883 us
    Day 25     221 us      798 us
    =============================
    Total:  403949 us  2899713 us

## Algorithms used

Here is a brief overview of the algorithms and theorems used to solve each problem. For more information checkout the source code - it contains a lot of comments.

### Day 01

Sort the input in linear time using bitset. From the bitset produce linked list represented as array - it's more convinient to traverse linked list than bitset.

### Day 02

Straightforward.

### Day 03

Don't solve the problem in one iteration. It's faster if it's done with less if-statements in the main loop.

### Day 04

Perfect hash functions for field types and eye colors exist. They were found using bruteforce. Try to do as little copy operations as possible.

### Day 05

Use bitmasks to determine the index of each seat in the plane. Use bitset to sort indexes (of seats) and find the missing one.

### Day 06

Both parts differ only in the way the bitmasks are accumlated.

### Day 07

Use hash map to build an hierarchy. Use hash set to determine uniqueness.

### Day 08

Memorize all pairs {branching instruction, accumulated value}. It's faster to start the calculation from a saved state instead of repeatedly reaching an already calculated state.
The bool array with all already executed instructions is one for the whole problem. The array stops early the execution of useless branches.

### Day 09

Use sliding window to quickly validate the next number.

### Day 10

Sort the input in linear time using bitset. For part 2 use dynamic programming.

### Day 11

Precompute neighbours list for each part. Keep track of the remaining seats for processing.

### Day 12

Straightforward.

### Day 13

Chinese reminder theorem for part 2.

### Day 14

Permutation generation with bitmasks.

### Day 15

Represent each number as index in bitset. This decreases the size of the array and higher quantity of elements will be placed inside the CPU's TLB cache.

### Day 16

Use bitmasks.

### Day 17

Allocate one continuous chunk of memory and index for 3D/4D in it - it's faster than indexing per dimension.
Cache once per cycle the total number of active neighbours for tiles with active state.

### Day 18

Implement own top to bottom parser. Push the found terminals in a stack until evaluation is possible. Once evaluation criterias are met evaluate and dump the result back in the stack.

### Day 19

There's no need of general grammar processors like Packrat or CYK. Instead use the properties of the given grammar and build own parser. Almost every line of the algorithm is explained with comments.

### Day 20

- Represent each side as 10 digit number. Cache pairs {side,tile} to enable fast retrieval of tile when matching a side.
- Represent the 8x8 core of each tile as 64-bit number. This enables very fast transformations(mirror vertically and horizontally, main diagonal flip, rotation) on the core using SWAR-wise delta swaps.
- For part 1 start with the first tile from the input and find the tile in the upper-left corner. From there match all other sides.
- For part 2 use the fast transformations on each tile's core to mirror horizontally, mirror, or rotate. Each 8-bits of a core of one tile are part of 96-bit long row.
  Represent this row as two numbers with 64 and 51 bits and search for the biggest part of the sea monster inside.

### Day 21

Represent each ingredient with it's name packed in 45 bits - 5 bits per letter. Hash each allergen with order preserving minimal hash function in the range [0,8].
Intersect ingredient sets to filter the {allergen,ingredient} pairs.

### Day 22

Limit the max possible moves by a magic number.

### Day 23

Represent a linked list as an array.

### Day 24

Represent each cell in axial coordinates. The third coordinate is derived easy by knowing the other two and using the property q+r+s=0.

### Day 25

Calculate discrete logarithm using baby-step giant-step algorithm that reverses the Diffie-Hellman encryption.
