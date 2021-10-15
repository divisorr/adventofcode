#include "common.h"
#include "fileio.h"

	//Sort the adapters in linear time using bitset.

	//Part 1 counts the 1 and 3 differences between each two consecutive nodes

	/*Part 2 reduces the problem in similar way as the lattice paths problem does - find path
	from adapter(0) to adapter(max). It's true that the path from adapter(k) to adapter(n) is
	the same as the paths from adapter(k) to adapter(n-1) plus the paths from adapter(n-1) to adapter(n)
	This looks like dynamic programming.

	Using the small example and sorting it we get: (0)-1-4-5-6-7-10-11-12-15-16-19-(22)
	Store the result of the calculations in an array with size 19 - the max number from the input.
	22 is the integrated adapter and it doesn't bring additional ways, because it's max+3.
	There's only one way too choose adapter 0 (the wall socket)=> arr[0] = 1
	There's only one way to go from (0) to 1=> arr[1] = 1
	The ways to go from adapter 1 to adapter 4 are: arr[4] = arr[4-1] + arr[4-2] + arr[4-3] = 1 => arr[4] = 1
	From 4 to 5: arr[5] = arr[5-1] + arr[5-2] + arr[5-3] = 1 => arr[5] = 1
	From 5 to 6: arr[6] = arr[6-1] + arr[6-2] + arr[6-3] = 2 => arr[6] = 2
	From 6 to 7: arr[7] = arr[7-1] + arr[7-2] + arr[7-3] = 4 => arr[7] = 4
	From 7 to 10: arr[10] = arr[10-1] + arr[10-2] + arr[10-3] = 4 => arr[10] = 4
	and so on... until arr[19] = 8

	Observation shows that on each step we use only the 3 previous calculations. The rest of the array is discarded.
	So there's no need to use an array - 3 numbers for storage and 1 for the intermediate result will suffice.
	*/

output day10(const char* input, size_t bytes)
{
	Fileio file(input, bytes);
	unsigned temp;
		//Use bitset to sort the arary of adapters in linear time
	uint64_t arr[4] = {0};
	while (!file.eof())
	{
		temp = file.ReadPositive<unsigned>();
		arr[temp>>6] |= (uint64_t)1 << (temp&63);	//temp>>6 == temp/64
		file.ignorenl();
	}

		//Current and previous adapters identified by their jolts
	unsigned cur, prev=0;
	unsigned cacheZero;
		//Part1 variables
		//As stated in the problem there is one integrated adapter with 3 jolts difference
	unsigned count1=0, count3=1;
		//Part2 variables
		//minus1 is 1, because there's only 1 way to go from the wall socket to the lowest jolt
	long long minus1=1, minus2=0, minus3=0, interm;
		//Solution
	for (unsigned idx=0; idx<4; ++idx)
	{
		cacheZero = idx*64;
		uint64_t adapter = arr[idx];
		while (adapter)
		{
				/*Restore the original number: 64*array_idx + bit_idx.
				64*array_idx can be cached because it is the same for all bits for the current array position*/
			cur = cacheZero + (unsigned)std::countr_zero(adapter);
			switch(cur-prev)
			{
				case 1:
					++count1;
						//Rotate the elements and prepare them for the next step
					interm = minus1 + minus2 + minus3;
					minus3=minus2; minus2=minus1; minus1=interm;
					break;
				case 2:
						/*1) minus1 holds the last positive result. Since minus2 should become zero minus3 should store the
						last result of minus1.
						  2) minus1 is incremented with minus2 only.
							3) minus2 becomes 0*/
					minus3 = minus1;
					minus1 += minus2;
					minus2 = 0;
					break;
				case 3:
					++count3;
						//minus1 holds the result and doesn't need to change. Reset the rest.
					minus3 = minus2 = 0;
					break;
			}
			prev = cur;
				//Remove the rightmost 1 bit
			adapter &= (adapter-1);
		}
	}
	return {count1*count3, minus1};
}
