#include "common.h"
#include "fileio.h"

output day06(const char* input, size_t bytes)
{
	Fileio file(input, bytes);
	size_t& pos = file.pos;

	unsigned answer1=0, answer2=0xFFFFFFFF;
	long long part1=0, part2=0;
	while (pos<bytes)
	{
		unsigned mask=0;
		while (input[pos] >= 'a')	//use operator '>' because eof might be reached
			mask |= 1<<(input[pos++]-'a');
		file.ignorenl();
		answer1 |= mask;
		answer2 &= mask;

			//if(input[pos] < 'a') is mem access violation for the last line because pos>bytes
		if(pos>bytes || input[pos] < 'a')
		{	//Start new group, but first resolve the answers of the previous group
			part1 += std::popcount(answer1);
			part2 += std::popcount(answer2);
			answer1=0;
			answer2=0xFFFFFFFF;
			file.ignorenl();
		}
	}
	return {part1, part2};
}
