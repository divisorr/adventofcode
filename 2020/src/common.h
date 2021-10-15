#pragma once

#include <tuple>
#include <bit>
#include <cstring>
#include <string>

struct output
{
	output(long long p1, long long p2)
		: part1(p1), part2(std::to_string(p2))
	{}
	output(long long p1, const std::string& str)
		: part1(p1), part2(str)
	{}
	long long part1;
	std::string part2;
};

output day01(const char* input, size_t bytes);
output day02(const char* input, size_t bytes);
output day03(const char* input, size_t bytes);
output day04(const char* input, size_t bytes);
output day05(const char* input, size_t bytes);
output day06(const char* input, size_t bytes);
output day07(const char* input, size_t bytes);
output day08(const char* input, size_t bytes);
output day09(const char* input, size_t bytes);
output day10(const char* input, size_t bytes);
output day11(const char* input, size_t bytes);
output day12(const char* input, size_t bytes);
output day13(const char* input, size_t bytes);
output day14(const char* input, size_t bytes);
output day15(const char* input, size_t bytes);
output day16(const char* input, size_t bytes);
output day17(const char* input, size_t bytes);
output day18(const char* input, size_t bytes);
output day19(const char* input, size_t bytes);
output day20(const char* input, size_t bytes);
output day21(const char* input, size_t bytes);
output day22(const char* input, size_t bytes);
output day23(const char* input, size_t bytes);
output day24(const char* input, size_t bytes);
output day25(const char* input, size_t bytes);
