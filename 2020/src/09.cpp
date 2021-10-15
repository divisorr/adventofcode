#include "common.h"
#include "fileio.h"

#include <unordered_set>

unsigned LoadData(long long* xmas, Fileio& file)
{
	unsigned cur=0;
	while (!file.eof())
	{
		xmas[cur++] = file.ReadPositive<long long>();
		file.ignorenl();
	}
	return cur;
}

size_t day9_p1(const long long* xmas, unsigned size)
{
	const size_t preamble=25;
	std::unordered_set<long long> diff;
	for (size_t pos=0; pos<preamble; ++pos)
		diff.emplace(xmas[pos]);
	for (size_t pos=preamble; pos<size; ++pos)
	{
		bool bFound = false;
		for (size_t idx=pos-preamble; idx<pos; ++idx)
		{
			if (diff.count(std::abs(xmas[pos]-xmas[idx])) != 0)
			{
				bFound = true;
				break;
			}
		}
		if(!bFound)
			return pos;
		diff.emplace(xmas[pos]);
		diff.erase(xmas[pos-preamble]);
	}
		//Unreachable
	return 0;
}

long long day9_p2(const long long* xmas, size_t pos)
{
	for (size_t idx=pos-1; idx!=0; --idx)
	{
		unsigned it=0;
		long long chain[25]{};
		long long accumulated = xmas[pos];
		for (size_t w_idx=idx; w_idx!=0; --w_idx)
		{
			accumulated -= xmas[w_idx];
			if (accumulated > 0)
				chain[it++]=xmas[w_idx];
			else if (accumulated < 0)
				break;
			else
			{
				chain[it++]=xmas[w_idx];
					//Find smallest and largest numbers
				long long smallest = chain[0];
				long long largest = chain[0];
				for (unsigned i=0; i<it; ++i)
				{
					if (chain[i] > largest)
						largest = chain[i];
					else if (chain[i] < smallest)
						smallest = chain[i];
				}
				return smallest + largest;
			}
		}
	}
		//Unreachable
	return 0;
}

output day09(const char* input, size_t bytes)
{
	long long xmas[1000]{};
	Fileio file(input, bytes);
	unsigned size = LoadData(xmas, file);
	size_t pos = day9_p1(xmas, size);
	long long part1 = xmas[pos];
	long long part2 = day9_p2(xmas, pos);
	return {part1, part2};
}
