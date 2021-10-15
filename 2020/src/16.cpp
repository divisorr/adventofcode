#include "common.h"
#include "fileio.h"

  //Max size of all rules
const static unsigned ruleSize = 20;

struct range
{
	unsigned min, max;
};

struct rule
{
	range ranges[2];
	bool IsValid(unsigned num) const
	{
		return (ranges[0].min <= num && num <= ranges[0].max) ||
		       (ranges[1].min <= num && num <= ranges[1].max);
	}
};

  //Returns bitmask with the index of each rule, that the number satisfies
inline unsigned ApplicableRules(unsigned num, const rule* rules)
{
	unsigned mask = 0;
	for (unsigned idx=0; idx<ruleSize; ++idx)
	{
		if (rules[idx].IsValid(num))
			mask |= (1 << idx);
	}
	return mask;
}

output day16(const char* input, size_t bytes)
{
	Fileio file(input, bytes);

	  //rules
	rule rules[ruleSize];
	range* curRange;
	for (unsigned idx=0; idx<ruleSize; ++idx)
	{
		curRange = rules[idx].ranges;
		while(file.text[file.pos++] != ':');
		  //whitespace
		file.ignore();
		curRange[0].min = file.ReadPositive<unsigned>();
		  //"-"
		file.ignore();
		curRange[0].max = file.ReadPositive<unsigned>();
		  //" or "
		file.ignore(4);
		curRange[1].min = file.ReadPositive<unsigned>();
		  //"-"
		file.ignore();
		curRange[1].max = file.ReadPositive<unsigned>();
		file.ignorenl();
	}
	  //empty new line between the rules and "your ticket:"
	file.ignorenl();

	  //"your ticket:"
	while(file.text[file.pos++] != ':');
	file.ignorenl();
	unsigned myTicket[ruleSize];
	for (unsigned idx=0; idx<ruleSize; ++idx)
	{
		myTicket[idx] = file.ReadPositive<unsigned>();
		if (file.text[file.pos] == ',')
			file.ignore();
	}
	file.ignorenl();
	  //empty new line between "your ticket" and "nearby tickets"
	file.ignorenl();

	  //nearby tickets
	while(file.text[file.pos++] != ':');
	file.ignorenl();
	  //Cache with how many rules this number satisfies.
	unsigned numCache[1000] = {0};
	  //Cache for the whole line
	unsigned ticketCache[ruleSize] = {0};
	  //& each ticketCache to get the bitmask of each column
	unsigned bitmask[ruleSize];
	memset(bitmask, 0xFF, ruleSize*sizeof(unsigned));
	unsigned errorRate=0;
	  //read nearby tickets:
	while (!file.eof())
	{
		bool bValidTicket = true;
		for (unsigned col=0,num; col<ruleSize; ++col)
		{
			num = file.ReadPositive<unsigned>();
			if (file.text[file.pos] == ',')
				file.ignore();
			if (!numCache[num] && (numCache[num]=ApplicableRules(num, rules))==0)
			{
				errorRate += num;
				bValidTicket = false;
			}
			ticketCache[col] = numCache[num];
		}
		file.ignorenl();
		if(bValidTicket)
		{
			for (unsigned col=0; col<ruleSize; ++col)
				bitmask[col] &= ticketCache[col];
		}
	}

	  //Part 2. bitmask now has the number of rules, that can be applied to each element;
	  //We should process the element with a single bit set, then with two bits etc..
	unsigned processingOrder[ruleSize];
	for (unsigned col=0; col<ruleSize; ++col)
		processingOrder[std::popcount(bitmask[col])-1] = col;
	long long prod = 1;
	  /*On each iteration store all already processed bits.
	  Then xor with the current bitmask to leave only the bit for the current rule.
	  The number of zeros after the remaining bit(that is the offset from the begining) is
	    the rule, that is satisfied by this column.
	  For the final result we're interested only in the first 6 rules.*/
	for (unsigned col=0, processsedColumns=0; col<ruleSize; ++col)
	{
		if (std::countr_zero(bitmask[processingOrder[col]] ^ processsedColumns) < 6)
			prod *= myTicket[processingOrder[col]];
		processsedColumns |= bitmask[processingOrder[col]];
	}

	return {errorRate, prod};
}
