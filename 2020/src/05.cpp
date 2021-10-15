#include "common.h"
#include "fileio.h"


output day05(const char* input, size_t bytes)
{
	Fileio file(input, bytes);
	size_t& pos = file.pos;

		//Bitset for sorting all seats and finding the missing seat for part2
	uint64_t bitset[16]{};
		//Currently parsing this line
	const char* cur;
	while (pos < bytes)
	{
		cur = input+pos;
		pos += 7;
		unsigned char r=0;
			//The accumulated row is bitwise inversed from the target row
		for (unsigned idx=0; idx<7; ++idx)
			r |= ((cur[idx]&0x04)>>2)<<(6-idx);
		cur = input+pos;
		pos += 3;
		unsigned char c=0;
			//The accumulated column is bitwise inversed from the target column
		for (unsigned idx=0; idx<3; ++idx)
			c |= ((cur[idx]&0x04)>>2)<<(2-idx);

			//Invert r/c to get the actual row/column. Only the lower 7/3 bytes are meaningful.
		unsigned id = (unsigned)(~r&127)*8 + (~c&7);
		bitset[id>>6] |= (uint64_t)1<<(id%64);
		file.ignorenl();
	}
		//Part 1
		//Get the number behind the highest bit in the bitset
	unsigned idx=15;
	while (bitset[idx] == 0)
		--idx;
	long long part1 = idx*64 + (std::popcount(bitset[idx])-1);
		//Part 2
		//The seats at the very front or at the back are missing (stated in the question)
	idx=0;
	while (bitset[idx++] == 0);
		//The first group of numbers that is not 0 has a high chance of having 0s at the least significant bit positions.
	idx += (bool)std::countr_zero(bitset[idx]);
	while (bitset[idx] == 0xFFFFFFFFFFFFFFFF)
		++idx;
	uint64_t num = ~bitset[idx];
		//This transforms all bits to the right, but doesn't touch the current bit, because it's 1 already
	num |= num-1;
		//The effect from above is countered here with -1
	long long part2 = idx*64 + (std::popcount(num)-1);

	return {part1,part2};
}
