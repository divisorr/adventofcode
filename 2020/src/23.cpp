#include "common.h"

/*  The purpose of the game is to pick 3 elements from an array and insert them
back in the array at some position. Linked list looks like a nice data structure
for the solution. The problem is that it's a bit slow when searching for element '1'
after all moves.
Instead we can simulate a "linked list" with just an array.
All elements are consecutive numbers. In the index position of each number could be
written the next element in the sequence. The last element should point to the first.

Example:  The sample input is 389125467. The array becomes a[0]=3 -> a[3]=8 ->
a[8]=9 -> a[9]=1 -> a[1]=2 ->etc... Don't forget to loop the array: ...->a[7]=3

The algorithm becomes:
1) Let's call element 't' the element past the last picked. 't' will be the new
   current, so it should come right after the current element.
2) then after element 't' should come the first from the picked elements.
3) the last from the picked elements should start pointing to the element after 't'.

Something needs to keep track of the "current" element too.*/

static inline void Solve(unsigned* cups, unsigned numCups, unsigned moves)
{
	unsigned cur;
	cur = cups[0];

	for (unsigned move=1; move<=moves; ++move)
	{
		unsigned f = cups[cur];
		unsigned s = cups[f];
		unsigned t = cups[s];
		unsigned dest = cur;
		do
		{
			--dest;
			if(dest==0)
				dest = numCups;
		} while(dest==f || dest==s || dest==t);
			//The steps from the algorithm.
			//Step 1)
		cur = cups[cur] = cups[t];
			/*Step 3) If step 2 is placed before step 3,
			  cups[dest] (that is the element after 'dest') will be incorrect,
				because it was already changed.*/
		cups[t] = cups[dest];
			//Step 2)
		cups[dest] = f;
	}
}

output day23(const char* input, size_t)
{
		//size of the array for each part of the challenge
	const unsigned p1_size = 9;
	const unsigned p2_size = 1000000;
		/*We need one more element in the arrays because it's nice to keep the indexes
		consistent with the element positions.*/
	unsigned p1_cups[p1_size+1];
	unsigned* p2_cups = new unsigned[p2_size+1];
	unsigned cur = 0;
		//The first 9 elements are the same for part1 and part2
	for (unsigned idx=0; idx<p1_size; ++idx)
		cur = p1_cups[cur] = p2_cups[cur] = input[idx]-'0';
	p1_cups[cur] = p1_cups[0]; //Make the list loop - the last element should point to the first

		//For part2 initialize the rest of the array with consecutive numbers
	for (unsigned idx=p1_size+1; idx<=p2_size; ++idx)
		cur = p2_cups[cur] = idx;
	p2_cups[cur] = p2_cups[0]; //Make the list loop - the last element should point to the first

		//Solutions
	Solve(p1_cups, p1_size, 100);
	Solve(p2_cups, p2_size, 10000000);

		//Extract the results. The first element after cup 1 is cups[1].
	long long res1 = p1_cups[1];
	for (unsigned idx=p1_cups[1]; p1_cups[idx]!=1; idx=p1_cups[idx])
		res1 = 10*res1 + p1_cups[idx];

		//The first element after cup 1 is cups[1]. The second is cups[cups[1]].
	long long res2 = (long long)p2_cups[1] * p2_cups[p2_cups[1]];

	delete [] p2_cups;
	return {res1,res2};
}
