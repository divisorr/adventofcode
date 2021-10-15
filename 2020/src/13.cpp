#include "common.h"
#include "fileio.h"

#include <limits.h>


/*
==Part 1 is straightforward

==Part 2
The answer for part2 uses the Chinese Remainder Theorem (CRT).
  The input for CRT is not given directly by the problem and should be derived.
  CRT states: A==a1(mod p1), A==a2(mod p2),..., A==ak(mod pk)
  p1,...,pk are given - the bus ids. We have to derive a1,...,ak and then find A through CRT.
  From the problem statement - we have to find: (A + i) (mod pi) = 0
  A == -i mod pi
  A == (pi - i) mod pi
  So ai is just (id - i)
*/
inline unsigned long long gcdx(unsigned long long a, unsigned long long b, long long& x, long long& y)
{
	if(!b)
	{
		x=1, y=0;
		return a;
	}
	long long prevX, prevY;
	unsigned long long gcd = gcdx(b, a%b, prevX, prevY);
	x = prevY;
	y = prevX - (a/b)*prevY;
	return gcd;
}

inline long long ModInv(unsigned long long a, unsigned long long b)
{
	long long x, y;
	gcdx(a, b, x, y);
	return x<0? x+b : x;
}

struct ID { unsigned mod, idx; };

output day13(const char* input, size_t bytes)
{
	Fileio file(input, bytes);
		//part1 data
	unsigned depart, cur_wait, min_wait=UINT_MAX, part1=0;
		//part2 data
	unsigned long long N=1;
	ID cache[20];
	unsigned cachePos=0;
		//solution
	depart = file.ReadPositive<unsigned>();
	file.ignorenl();
	for (unsigned idx=0,id; !file.eof(); ++idx)
	{
		if(file.text[file.pos] == 'x')
			file.ignore(2);  //"x,"
		else
		{
			id = file.ReadPositive<unsigned>();
			file.ignore();  //","
			cur_wait = id - depart%id;
			if (cur_wait < min_wait)
			{
				min_wait = cur_wait;
				part1 = id*cur_wait;
			}
			cache[cachePos++] = {id, idx};
			N *= id;
		}
	}
		//part2
	long long part2=0, yi;
	for (size_t idx=0; idx<cachePos; ++idx)
	{
		long long mod = cache[idx].mod;
		yi = N / mod;
			//Find modular multiplicative inverse
		part2 += (mod-cache[idx].idx) * yi * ModInv(yi, mod);
	}
	part2 %= N;
	return {part1,part2};
}
