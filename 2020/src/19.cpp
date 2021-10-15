//The current solution is based on: https://github.com/Voltara/day2020-fast/blob/main/src/day19.cpp

#include "common.h"
#include "fileio.h"

#include <vector>
#include <bitset>


	//Structure for holding the initial graph of rules, as given by the input.
struct Rule
{
		/*The name 'nonterminal' is misleading. It mainly stores nonterminals.
		'a' and 'b' are terminals and are stored at index 1 in the array leaving the array at index 0 empty.
		There's no way in which this would happen with the rest of the input rules so it's safe method for distinguishing between
		terminals and nonterminals.
		The rules on the left of | are always stored on pos 0 and 1.
		The rules on the right of | are always stored on pos 2 and 3.*/
	unsigned nonterminal[4]{};
};

	//Structure for holding rule word for a rule after traversing the rules hierarchy
struct Word
{
		//bitset mask which stores the built rule. Range [0;255]
	unsigned mask;
		//number of bits used - there can be leading zeros which are part of the rule.
	unsigned len;
};

class Solver19
{
public:
	void ReadRules(Fileio& file);
	output Solve(Fileio& file);
private:
	void BuildAllPatterns(unsigned idx);
private:
		//The raw input of rules fitted
	Rule rules[140]{};
		//For each rule from the array there is a vector of rule words
	std::vector<Word> words[140]{};
};

inline void Solver19::ReadRules(Fileio& file)
{
	const char* input = file.text;
	size_t& pos = file.pos;
	while (input[pos] > file.nl)
	{
		unsigned rulePos = file.ReadPositive<unsigned>();
		file.ignore(2); //": "
		if (input[pos] == '"')
		{ //This is terminal
			file.ignore();	//ignore quote
				//store the terminal at index 1 leaving the array at index 0 uninitialized
			rules[rulePos].nonterminal[1] = input[pos] & 1;
			file.ignore(2);	//ignore the terminal character and the following closing quote
			file.ignorenl();
		}
		else
		{	//This is nonterminal
			unsigned idx=0;
			while (input[pos] > file.nl)
			{
				if (input[pos]==' ')
					file.ignore();
				else if (input[pos]=='|')
				{
						//The rule(s) on the right of | always take indexes 2 (and 3) regardless of the number of rules on the left of |.
					if (idx == 1)
						++idx; //Don't put rule on the right of | in position 1
					file.ignore();
				}
				else
				{
					rules[rulePos].nonterminal[idx++] = file.ReadPositive<unsigned>();;
				}
			}
			file.ignorenl();
		}
	}
}

inline void Solver19::BuildAllPatterns(unsigned idx)
{
	if (words[idx].empty())
	{
		unsigned* nt = rules[idx].nonterminal;
		if (nt[0] == 0)
		{
				//This is terminal
			words[idx].emplace_back(Word{nt[1],1});
		}
		else
		{
				//This is nonterminal
			for (unsigned ntIdx=0; ntIdx<4 && nt[ntIdx]; ntIdx+=2)
			{
					//Recursively dive
				unsigned num1 = nt[ntIdx];
				BuildAllPatterns(num1);
				unsigned num2 = nt[ntIdx+1];
				BuildAllPatterns(num2);
					//Make all possible combinations of characters where the second set of characters follows the first
				for (Word w1 : words[num1])
					for (Word w2: words[num2])
						words[idx].emplace_back(Word{(w2.mask<<w1.len)|w1.mask, w1.len+w2.len});
			}
		}
	}
}

output Solver19::Solve(Fileio& file)
{
	const char* input = file.text;
	size_t& pos = file.pos;
		/*Rule 0 is always 8 11, but 8 and 11 cause loops in the grammar.
		To parse the rules in recursive manner rule 0 has to be ignored.*/
	words[0].emplace_back(Word());
		/*Rule 0 is always 8 11; 8 is always 42; rule 11 is always 42 31.
		=> rule 0 is 42 42 31.
		After the rules are built they contain only the terminals 'a' and 'b' (1 and 0).
		It turns out that rules 31 and 42 each have 128 rule words. It's convenient to represented a rule word as 8bit number.
		This way the rule words for both rules (31 and 42) are supplementing each other in the range [0;255].
		So it's enough to build only one of the rules (31 or 42) as the values that are not part of the first will be part of the latter.*/
	BuildAllPatterns(42);
		/*All rules built by 42 are numbers in the range [0;255]. They can be represented as bitset
		which allows very fast check whether an 8 char chunk from the message obbeys some rule of 42*/
	std::bitset<256> patterns;
	for (Word& w : words[42])
		patterns[w.mask]=1;

	long long part1=0, part2=0;
	file.ignorenl();	//nl between Rules and Messages
		/*Start processing all received messages. There is no need to check a whole message (1 line) at once
		against all rule words of all rules. Instead split the message into chunks of 8 chars each.
		Convert each chunk to a number in the range [0;255] and see if it was already built by rule 42 or by rule 31*/
	while(!file.eof())
	{
			//How many times for this message there was a transition from 42 to 31 rule or from 31 to 42
		unsigned transitioned=0;
			//Number of valid chunks for each ruleset for this message
		unsigned valid42=0, valid31=0;
		while (input[pos] > file.nl)
		{
			unsigned chunk=0;
				//convert the chunk to a number in the range [0;255]
			for (unsigned idx=0; idx<8; ++idx)
				chunk |= (input[pos++]&1)<<idx;
				//check if the chunk is validated by some rule of 42
			bool valid42Chunk = patterns[chunk];
				//increment the corresponding
			valid42 += valid42Chunk;
			valid31 += !valid42Chunk;
				//Has the previous chunk satisfied the same rule? True if it hasn't.
			transitioned += !(valid42Chunk^(bool)transitioned);	//true if both are true or both are false
		}
			/*The message is considered valid for part1 if several criteria are met:
			1) The message should contain both rules 42 and 31 - only one switch from 42 to 31 is allowed.
			2) By definition messages satisfying rule 0 should satisfy 42 42 31. => There must be exactly
			   two valid chunks for 42 and one valid chunk for 31.*/
		part1 += (transitioned==1) && (valid42==2) && (valid31==1);
			/*The definitions for part 2 bring a small change:
			1) The message should still contain both rules 42 and 31 - one switch from 42 to 31 is allowed.
			2) For 8: 42 must be satisfied at least once, possibly multiple times.
			   For 11: there can be multiple valid 42 chunks which should be followed by an equal number of valid 31 chunks.
			   => 0: 8 11 translates to multiple valid 42 chunks and valid 31 chunks. So 42 chunks > 31 chunks is enough.*/
		part2 += (transitioned==1) && (valid42>valid31);
		file.ignorenl();
	}
	return {part1,part2};
}

output day19(const char* input, size_t bytes)
{
	Fileio file(input, bytes);
	Solver19 s;
	s.ReadRules(file);
	return s.Solve(file);
}
