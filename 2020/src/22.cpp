#include "common.h"
#include "fileio.h"

struct Hand
{
	unsigned hand[50];
	unsigned size;
};

static inline void ReadHand(Hand& player, Fileio& file)
{
	file.ignore(9); //'Player 1:'
	file.ignorenl();

	unsigned* hand = player.hand;
	for (unsigned idx=0; idx<25; ++idx)
	{
		hand[idx] = file.ReadPositive<unsigned>();
		file.ignorenl();
	}
	player.size = 25;
}

static inline long long CalcHand(const Hand& winner)
{
	unsigned mult = 50;
	unsigned sum = 0;
	const unsigned* hand = winner.hand;
	for (unsigned idx=0; idx<50; ++idx)
		sum += (mult--)*hand[idx];
	return sum;
}

static inline void PlayGame(Hand& p1, Hand& p2)
{
	unsigned* hand1 = &p1.hand[0];
	unsigned* hand2 = &p2.hand[0];
	unsigned& size1 = p1.size;
	unsigned& size2 = p2.size;
	const unsigned typeSize = sizeof(*hand1);

	while (size1 && size2)
	{
		unsigned c1 = hand1[0];
		unsigned c2 = hand2[0];
		memmove(hand1, &hand1[1], typeSize*(size1-1));
		memmove(hand2, &hand2[1], typeSize*(size2-1));
		if (c1 > c2)
		{
			hand1[size1-1] = c1;
			++size1;
			hand1[size1-1] = c2;
			--size2;
		}
		else
		{
			hand2[size2-1] = c2;
			++size2;
			hand2[size2-1] = c1;
			--size1;
		}
	}
}

	//Returns true when player 1 wins
static inline bool Round2(Hand& p1, Hand& p2)
{
	unsigned* hand1 = &p1.hand[0];
	unsigned* hand2 = &p2.hand[0];
	unsigned& size1 = p1.size;
	unsigned& size2 = p2.size;
	const unsigned typeSize = sizeof(*hand1);
	bool bWinner;

	unsigned maxRounds = 1400;
	while (maxRounds && size1 && size2)
	{
		unsigned c1 = hand1[0];
		unsigned c2 = hand2[0];
		if (c1 < size1 && c2 < size2)
		{
			Hand sub1, sub2;
			memcpy(sub1.hand, &hand1[1], typeSize*c1);
			sub1.size = c1;
			memcpy(sub2.hand, &hand2[1], typeSize*c2);
			sub2.size = c2;
			bWinner = Round2(sub1, sub2);
		}
		else
		{
			bWinner = c1>c2;
		}

		memmove(hand1, &hand1[1], typeSize*(size1-1));
		memmove(hand2, &hand2[1], typeSize*(size2-1));
		if (bWinner)
		{
			hand1[size1-1] = c1;
			++size1;
			hand1[size1-1] = c2;
			--size2;
		}
		else
		{
			hand2[size2-1] = c2;
			++size2;
			hand2[size2-1] = c1;
			--size1;
		}
		--maxRounds;
	}
	if (maxRounds==0 || size1>0)
		return true;
	else
		return false;
}

output day22(const char* input, size_t bytes)
{
	Fileio file(input, bytes);
	Hand p1, p2;
	ReadHand(p1, file);
	file.ignorenl();
	ReadHand(p2, file);

	Hand r1=p1, r2=p2;
	PlayGame(p1, p2);
	Round2(r1, r2);

	long long res1 = CalcHand(p1.size? p1 : p2);
	long long res2 = CalcHand(r1.size? r1 : r2);
	return {res1,res2};
}
