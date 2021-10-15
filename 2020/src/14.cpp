#include "common.h"
#include "fileio.h"

#include <bitset>
#include <algorithm>


struct memory
{
	uint64_t address;
	uint64_t value;
	bool operator<(const memory& rhs) const { return address < rhs.address; }
};

output day14(const char* input, size_t bytes)
{
	Fileio file(input, bytes);
		//Storage for all memory assignments
	memory* memory_p1 = new memory[1000];
	memory* memory_p2 = new memory[100000];
		//Current operation
	char op;
		//Masks with all explicit 0 and 1, no X. Note, that the mask with 0 is stored inverted.
	uint64_t mask0=0, mask1=0;
		//Current address and value
	uint64_t address, value;
	int mem1Idx=0;
	int mem2Idx=0;
	while (!file.eof())
	{
		file.ignore(1);
		op = file.text[file.pos++];
		switch(op)
		{
		case 'a':
			mask0 = mask1 = 0;
			file.ignore(5);
			while (file.text[file.pos] != file.nl)
			{
				mask0 <<= 1;
				mask1 <<= 1;
				if (file.text[file.pos] == '0')
					++mask0;
				else if (file.text[file.pos] == '1')
					++mask1;
				++file.pos;
			}
			break;
		case 'e':
			file.ignore(2);
			address = file.ReadPositive<unsigned>();
			file.ignore(4);
			value = file.ReadPositive<unsigned>();
				//For part 1 simply push back the memory
			memory_p1[mem1Idx++] = {address, (value&~mask0)|mask1};
				/*For part 2 generate all permutations of X bits, sum them with the base address
				and store each permutation. The array might contain duplicated addresses but this
				will be taken care of later*/
			uint64_t inverted_x_mask = mask0 | mask1;
			uint64_t x_mask = 0xfffffffff ^ inverted_x_mask;
			address = (address&mask0)|mask1;
			uint64_t purmutation=0;
			do
			{
				memory_p2[mem2Idx++] = {address+purmutation, value};
					/*This algorithm creates lexicographical perumtations.
					We have a bitmask.
					1) The first permuation is 0.
					2.1)The next permutation is the previous permutation plus
					    bitmask of 1 bits. The 1 bits are on inverse positions compared
					    with the target bitmask.
					2.2)The result is then incremented with 1 to fill all lower bits - lower than the
					    highest bit position in the previous permutation - and add 1 bit on a higher position.
					2.3)Finally & x_mask removes all 1 bits, that should not be part of the permutation.
					*/
				purmutation = ((purmutation + inverted_x_mask) + 1) & x_mask;
			} while(purmutation);
			break;
		}
		file.ignorenl();
	}

		//Part1 can count only the last occurence
	uint64_t res_p1 = 0;
	std::bitset<100000> bitset;
	for (int idx=mem1Idx-1; idx>=0; --idx)
	{
		if(bitset[memory_p1[idx].address] == 0)
		{
			bitset[memory_p1[idx].address] = 1;
			res_p1 += memory_p1[idx].value;
		}
	}

		/*The addressses for part2 are huge and the program runs out of stack if
		  you try to create a bitset. Instead sort the array by keeping the
		  relative position of the equal elements and then simply iterate over the
		  different neighbours from the array.
		  The sorting is the slowest part of the whole solution to this problem.
			Sorting and checking for consecutive duplicates is faster than using unordered_set*/
	uint64_t res_p2 = 0;
	std::stable_sort(memory_p2, memory_p2+mem2Idx);
	uint64_t prev=0;
	for (int idx=mem2Idx-1; idx>=0; --idx)
	{
		if (prev != memory_p2[idx].address)
		{
			prev = memory_p2[idx].address;
			res_p2 += memory_p2[idx].value;
		}
	}
	delete [] memory_p1;
	delete [] memory_p2;
	return {(long long)res_p1, (long long)res_p2};
}
