#include "common.h"
#include "fileio.h"

#include <unordered_map>
#include <cmath>

  //Today's puzzle has only one part
/*Using the given examples' values the problem can be restated like this:
	Take a number (7) and raise it to a power(8 for card; 11 for door). The result is the primary key for the
card or door. Take the pk of the door and rise it to the power equal to the card's loop size(8).
Alternatevly take the card's pk and raise it to the power of the door's loop size. Either will give the same encryption key.
	7^11=door_pk; door_pk^8=encryption key, so the encryption key is 7^(11+8) = 7^19
  The problem could be simplified further: given a and b find x such that a^x = b. The obvious solution is to find
x = loga(b), but the problem states there is modular arithmetic involved. When modular arithmetic is involved the
logarithm is called descrete logarithm.
	The implemented algorithm is baby-step giant-step which calculates the descrete logarithm. The discrete log reverses
the Diffie-Hellman encryption from the problem statement. If the module wasn't a prime number then Pohlig–Hellman's algorithm
would have been used.*/


typedef long long ll;

	//binary modular exponentiations
static inline ll bme(ll num, ll pow, ll mod)
{
	ll res = 1;
	while (pow)
	{
		if (pow&1)
			res = (res*num) % mod;
		num = (num*num) % mod;
		pow >>= 1;
	}
	return res;
}

	//baby-step giant-step
static inline ll bsgs(ll a, ll b, ll mod)
{
		//Find the descrete log of a^x=b
	/*Algorithm from Wikipedia's Baby-step giant-step article:
	Represent x as im+j, where m=ceil(sqrt(x)) and 0<=i,j<m
	Then a^j = b*((a^-m)^i).
	  Steps:
	    1. Precompute a^j for all values of j and store the pairs {a^j, j}
	    2. Calculate c = a^-m
	    3. For every i < m
	      3.1. check if b is a key from the pairs found in step 1
	      3.2. if yes => the answer is x=i*m+j
	      3.3. if no => b = b*c*/

	//Step 1
	ll m = (ll)sqrt((unsigned)mod)+1;
		/*No need to use hash_map. Simple array will suffice but hash_map
	increases computation because it uses a lot less memory*/
	std::unordered_map<ll,ll> store;
	ll res = 1;
	for (ll j=0; j<m; ++j)
	{
		store.insert({res, j});
		res = (res * a) % mod;
	}

	//Step 2
	/*We are looking for modular multiplicative inverse.
	In other words we are looking for x such that:
	((a^m) * a^x)(mod p) = 1(mod p);
	(a^(m + x))(mod p) = 1(mod p);

	From Fermat's little theorem we know that:
	a^p=a(mod p), which means that a^(p-1) = 1(mod p);

	Substituting the first equation in Fermat's theorem:
	a^(m+x) (mod p) = a^(p-1) (mod p);
	m+x=p-1 => x=p-m-1
	*/
	ll c = bme(a, mod-m-1, mod);

	//Step 3
	ll x=0;
	for (ll i=0; i<m; ++i)
	{
		if (store.find(b)!=store.end())
		{
			x = i*m + (*store.find(b)).second;
			break;
		}
		else
			b = (b*c) % mod;
	}
	return x;
}

output day25(const char* input, size_t bytes)
{
	Fileio file(input, bytes);
	ll door_pk = file.ReadPositive<ll>();
	file.ignorenl();
	ll card_pk = file.ReadPositive<ll>();
	ll mod = 20201227;

	ll card_loop = bsgs(7, card_pk, mod);
	long long p1_res = bme(door_pk, card_loop, mod);
	return {p1_res, 0};
}
