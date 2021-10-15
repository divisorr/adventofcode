#include "common.h"
#include "fileio.h"

#include <set>
#include <map>
#include <algorithm>
#include <iterator>


output day21(const char* input, size_t bytes)
{
	Fileio file(input, bytes);
	size_t& pos = file.pos;

		//Association between an allergen and all possible ingredients
	std::map<uint64_t, std::set<uint64_t>> allergen2Ingr;
		//unique ingredients and their count
	std::map<uint64_t, unsigned> ingr2Num;
	unsigned ingrCount=0;
	while (!file.eof())
	{
		std::set<uint64_t> ingredients;
			//parse ingredients
		while (input[pos] != '(')
		{
				/*1) Each ingredient consists of at most 9 lowercase letters
				  2) there are 26 letters => each letter takes at most 5 bits
				From 1) and 2) => each ingredient can be represented in 9*5 bits.*/
			uint64_t ingredient = 0;
			for (unsigned idx=0; input[pos] >= 'a'; )
			{
				ingredient |= (uint64_t)(input[pos++]-'a') << (5*idx++);
			}
			ingredients.emplace(ingredient);
			++ingr2Num[ingredient];
			++ingrCount;
			file.ignore();	//" "
		}

		file.ignore(10); //"(contains "

			//parse allergens
		while (input[pos] != ')')
		{
				//Represent each allergen as number.
			uint64_t allergen=0;
			for(unsigned idx=0; input[pos] >= 'a'; ++idx)
			{
				allergen |= (uint64_t)(input[pos++]-'a') << (5*(9-idx));
			}
				//Order preserving minimal hash function. This hashes the allergen in the range [0,9).
			allergen = allergen % 19742155 % 9;
			std::set<uint64_t>& ingr = allergen2Ingr[allergen];
			if (ingr.size() == 0)	//If this is the first time this allergen is met
			{
					//Copy all ingredients
				ingr = ingredients;
			}
			else
			{
					//Don't trust this sentence of the puzzle: "Allergens aren't always marked".
					//Keep the intersection of the existing elements and the new elements.
				std::set<uint64_t> temp;
				std::set_intersection(ingr.begin(), ingr.end(), ingredients.begin(), ingredients.end(), std::inserter(temp, temp.end()));
				ingr = std::move(temp);
			}
			if (input[pos]==',')
				file.ignore(2); //", "
		}

		file.ignore();	//")"
		file.ignorenl();
	}

		//Some allergens still contain more than one ingredient. The result of this piece of code is one on one mapping.
	uint64_t oneOnOne[9]{};
	while (!allergen2Ingr.empty())
	{
			//Find an allergen with 1 ingredient.
		auto it=allergen2Ingr.begin();
		while((*it).second.size()!=1)
			++it;
		uint64_t num = *(*it).second.begin();
		oneOnOne[(*it).first] = num;
		allergen2Ingr.erase(it);
			//Remove the ingredient from all other allergens.
		for (auto& i : allergen2Ingr)
			i.second.erase(num);
	}

		//For part1 sum the count of all allergic ingredients and subtract them from the total number of ingredients.
	unsigned allergicIngrCount=0;
		//For part2 accumulate the allergic ingerdients in a string.
	char part2[100]{};
	unsigned cur=0;
	for (uint64_t num : oneOnOne)
	{
		if (num)	//without this if-statement there will be one additional comma in the final answer
		{
			allergicIngrCount += ingr2Num[num];
				//The letters of the ingredients are stored in reverse order. Process the number in reverse order too to get the answer straight.
			while (num)
			{
				part2[cur++] = (num & 0x1F)+'a';	//0x1F == minor 5 bits
				num >>= 5;
			}
			part2[cur++] = ',';
		}
	}
	part2[cur-1] = 0;
	long long part1 = ingrCount-allergicIngrCount;
	return {part1,part2};
}
