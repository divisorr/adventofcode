#include "common.h"
#include "fileio.h"

/*Algorithm: Each number from the input is stored as index in bitset. This gives O(n) time to sort the numbers.
For easier iteration over the elements all numbers are inserted in linked list represented with array.
To find the third number for part2 look for the difference 2020-(first+second). If first+second>2020 then third
number doesn't exist for this pair*/
output day01(const char* input, size_t bytes)
{
	long long part1=0, part2=0;

	Fileio file(input, bytes);
	unsigned elements=0;	//How many elements are there?
		//Chaining 32 ulls gives us numbers up until 32*64 = 2048 < 2020
		/*The position in the array represents a range [0,63], [64-127], [128-191], [192, 255] etc...
			The set bit in each cell identifies uniquely the number.
			e.g. 137 will be in arr[2], because 137/64 == 2;
			     in arr[2] bit 137%64 should be set.
			     To restore a number from the array it's enough to do arr_idx*64 + bit_idx
		*/
	uint64_t bitset[32] = {};
	while (!file.eof())
	{
		unsigned num = file.ReadPositive<unsigned>();
		bitset[num>>6] |= (uint64_t)1<<(num%64);	//num>>6 == num/64
		file.ignorenl();
		++elements;
	}

		//Dump the sorted numbers in a linked list represented as array. The first element is always arr[0]
		//Example: Let's have these numbers 13, 37, 48, 93. The array becomes:
		//arr[0]=13, arr[13]=37, arr[37]=48, arr[48]=93, arr[93]=0
	unsigned arr[2049]{};
		//The first number is always at index 0
	unsigned prev = 0;
	for (unsigned group=0; group<32; ++group)
	{
		uint64_t num = bitset[group];
		while (num)
		{
			prev = arr[prev] = (group<<6) + std::countr_zero(num);
			num &= (num-1);
		}
	}

	const unsigned limit = 2020;
		//Part1
	for (unsigned num=arr[0]; num; num=arr[num])
	{
		if (arr[limit-num])
		{
			part1 = num*(limit-num);
			break;
		}
	}

		//Part2
	for (unsigned first=arr[0]; first; first=arr[first])
	{
		for (unsigned second=arr[first]; second; second=arr[second])
		{
			if (first+second >= limit)
				break;
			else if (arr[limit-(first+second)])
			{
				part2 = first * second * (limit-(first+second));
				first = 2048; //break the outer loop
				break;
			}
		}
	}

	return {part1, part2};
}
