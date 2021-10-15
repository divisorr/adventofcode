#include "common.h"
#include "fileio.h"

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

struct bag
{
	std::vector<bag*> parents; //who owns this bag
	std::unordered_map<bag*, unsigned> children; //what this bag owns and how much
};

typedef std::unordered_map<std::string,bag> BagMap;

inline std::string ParseBag(Fileio& file)
{
	size_t beg, size;
		//Saturation
	beg = file.pos;
	size = file.AdvanceString();
	file.ignore();
		//Hue
	size += file.AdvanceString();
	return std::string(file.text+beg, size+1);  //+1 becase we ignored the whitespace between the two words
}

bool LoadInput(BagMap& rMap, Fileio& file)
{
	std::string bagDescr;
	unsigned qty;
	while (!file.eof())
	{
			//Bag description
		bagDescr = ParseBag(file);
			//" bags contain "
		file.ignore(14);
		bag* curBag = &(*rMap.emplace(std::make_pair(bagDescr, bag())).first).second;
		if (file.text[file.pos] != 'n')  //"no other bags." is a possible scenario
		{
			while (1)
			{
					//Quantity. Single digit.
				qty = file.text[file.pos++] - '0';
					//whitespace
				file.ignore();
					//Bag description
				bagDescr = ParseBag(file);
				file.ignore();
				bag* subBag = &(*rMap.emplace(std::make_pair(bagDescr, bag())).first).second;
				subBag->parents.push_back(curBag);
				curBag->children.emplace(std::make_pair(subBag, qty));
					//"contains"
				file.AdvanceString();
				if (file.text[file.pos-1] == ',')
					file.ignore();  //whitespace
				else
				{
					file.ignorenl();
					break;
				}
			}
		}
		else
		{
			file.ignore(14);
			file.ignorenl();
		}
	}
	return true;
}

void dofind1(const bag* curbag, std::unordered_set<bag*>& uniqueNodes)
{
	for (auto& it : curbag->parents)
	{
		uniqueNodes.emplace(it);
		dofind1(it, uniqueNodes);
	}
}

size_t dofind2(const bag* curbag)
{
	size_t overall = 0;
	for (auto& it : curbag->children)
	{
		size_t qty = dofind2(it.first);
		overall += it.second * (qty + 1);
	}
	return overall;
}

output day07(const char* input, size_t bytes)
{
	Fileio file(input, bytes);
	BagMap bagMap;
	LoadInput(bagMap, file);

	const bag* curbag = &(*bagMap.find("shiny gold")).second;
	std::unordered_set<bag*> uniqueNodes;
	dofind1(curbag, uniqueNodes);
	long long part1 = uniqueNodes.size();
	long long part2 = dofind2(curbag);

	return {part1, part2};
}
