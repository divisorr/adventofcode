#include "common.h"
#include "fileio.h"

output day03(const char* input, size_t bytes)
{
	Fileio file(input, bytes);
	const size_t eof = file.size;
	const size_t size = file.AdvanceString();
	const size_t sizenl = size+file.nlSize;

	auto CountTrees = [input, size, sizenl, eof](unsigned right, unsigned bottom)
	{
		long long trees = 0;
		unsigned line = 0;
		for (unsigned offset=0; line*sizenl<eof; offset=(offset+right)%size)
		{
			trees += input[line*sizenl + offset]=='#';
			line += bottom;
		}
		return trees;
	};

	size_t trees1 = CountTrees(3, 1);
	size_t trees2 = CountTrees(1, 1);
	size_t trees3 = CountTrees(5, 1);
	size_t trees4 = CountTrees(7, 1);
	size_t trees5 = CountTrees(1, 2);

	long long part1 = trees1;
	long long part2 = trees1*trees2*trees3*trees4*trees5;

	return {part1, part2};
}
