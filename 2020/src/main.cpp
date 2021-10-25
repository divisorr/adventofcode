#include "common.h"

#include <chrono>
#include <iostream>
#include <iomanip>


static const char* LoadInput(const char* pszFileName, size_t& bytes)
{
	FILE* file = fopen(pszFileName, "rb");
	bytes = 0;
	if (!file)
		return nullptr;
	if (fseek(file, 0, SEEK_END) != 0)
	{
		fclose(file);
		return nullptr;
	}
	size_t size = ftell(file);
	if (size > 0)
	{
		char* input = new char[size];
		fseek(file, 0, SEEK_SET);
		size_t read = fread(input,sizeof(char),size,file);
		fclose(file);
		if (read != size)
		{
			delete [] input;
			return nullptr;
		}
		else
		{
			while (input[size-1] <= ' ')	//Skip whitespaces & newlines
				--size;
			bytes = size;
			return input;
		}
	}
	else
	{
		fclose(file);
		return nullptr;
	}
}

static void FreeInput(const char* input)
{
	delete [] input;
}


int main()
{
	std::cout << "          Time      Part 1          Part2" << std::endl
	          << "===============================================" << std::endl;

	output(*days[])(const char*, size_t) =
	{        day01, day02, day03, day04, day05, day06,
	  day07, day08, day09, day10, day11, day12, day13,
	  day14, day15, day16, day17, day18, day19, day20,
	  day21, day22, day23, day24, day25
	};

	const unsigned nameSize=255;
	char filename[nameSize];
	size_t bytes=0;
	std::chrono::steady_clock::time_point start, end;
	std::chrono::duration<double> span(.0), total(.0);
	for (unsigned idx=0; idx<sizeof(days)/sizeof(*days); ++idx)
	{
		sprintf(filename, "%02d.txt", idx+1);
		if (const char* input = LoadInput(filename, /*out*/bytes))
		{
			start = std::chrono::steady_clock::now();
			output result = days[idx](input, bytes);
			end = std::chrono::steady_clock::now();
			span = end-start;
			total += span;
			std::cout << "Day " << std::right << std::setfill('0') << std::setw(2) << idx+1
			          << std::setfill(' ') << std::setw(8) << std::chrono::duration_cast<std::chrono::microseconds>(span).count() << " us"
			          << "   " << std::left << std::setw(15) << result.part1 << " " << std::setw(20) << result.part2 << " " << std::endl;
			FreeInput(input);
		}
	}
	std::cout << "===============================================" << std::endl
	          << "Total:" << std::right << std::setw(8) << std::chrono::duration_cast<std::chrono::microseconds>(total).count() << " us\n"
	          << "Press ENTER to exit...";
	std::cin.get();
	return 0;
}
