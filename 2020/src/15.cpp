#include "common.h"
#include "fileio.h"

inline unsigned solve(unsigned from, unsigned to, unsigned* seen, uint64_t* cache, unsigned current)
{
	unsigned addr = 0;
	for (unsigned idx=from; idx<to; ++idx)
	{
		if (cache[current/64] & ((uint64_t)1<<(current&63)))
		{
			addr = current;
			current = idx-seen[addr];
			seen[addr] = idx;
		}
		else
		{
			cache[current/64] |= (uint64_t)1<<(current&63);
			seen[current] = idx;
			current = 0;
		}
	}
	return current;
}

output day15(const char* input, size_t bytes)
{
	const unsigned size_p1 = 2020;
	const unsigned size_p2 = 30000000;
		//The array with all numbers. It's too huge to allocate it on the stack.
	unsigned *seen = new unsigned[size_p2];
	memset(seen, 0, size_p2*sizeof(*seen));
		/*The cache will be used as bitmask - it contains 64 times less memory than the array with numbers
		  and in theory a big chunk of it should fit occasionally in the CPU's TLB cache.*/
	constexpr unsigned cache_size = size_p2/sizeof(uint64_t)+1;
	uint64_t *cache = new uint64_t[cache_size];
	memset(cache, 0, cache_size*sizeof(uint64_t));

	unsigned number;
	Fileio file(input, bytes);
	unsigned idx=1;
	while(!file.eof())
	{
		number = file.ReadPositive<unsigned>();
		file.ignore(); //","
		seen[number] = idx++;
		cache[0] |= (uint64_t)1<<number;
	}

	long long part1 = solve(idx, size_p1, seen, cache, 0);
	long long part2 = solve(size_p1, size_p2, seen, cache, (unsigned)part1);

	delete [] seen;
	delete [] cache;
	return {part1, part2};
}
