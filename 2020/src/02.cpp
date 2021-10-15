#include "common.h"
#include "fileio.h"


output day02(const char* input, size_t bytes)
{
	long long part1=0, part2=0;

	Fileio file(input, bytes);
	unsigned min, max;
	char c;
	size_t& pos = file.pos;
	unsigned count;
	while(!file.eof())
	{
			//Read
		min = file.ReadPositive<unsigned>();
		file.ignore(); //'-'
		max = file.ReadPositive<unsigned>();
		file.ignore(); //' '
		c = input[pos++];
		file.ignore(2); //': '
			//Part 2
		part2 += (input[pos+min-1]==c) ^ (input[pos+max-1]==c);
			//Part 1
		count = 0;
		while(input[pos] >= 'a')
			if (input[pos++]==c)
				++count;
		if (min<=count && count<=max)
			++part1;
		file.ignorenl();
	}
	return {part1, part2};
}
