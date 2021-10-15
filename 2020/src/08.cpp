#include "common.h"
#include "fileio.h"

struct instruction
{
	char op;	//a=acc, j=jmp, n=nop
	int val;
};

struct alternate
{
	unsigned instr;
	long long acc;
};

void LoadInput(Fileio& file, instruction* code, unsigned& lines)
{
	lines = 0;
	char sign;
	size_t& pos = file.pos;
	const char* text = file.text;
	char op;
	int val;
	while (!file.eof())
	{
		op = text[pos];
		file.ignore(4);
		sign = text[pos++];
		val = file.ReadPositive<unsigned>();
		if (sign == '-')
			val = -val;
		code[lines++] = {op,val};
		file.ignorenl();
	}
}

std::tuple<long long,bool> day8_p1(const instruction* code, unsigned lines, alternate* alt, unsigned instr, bool* parsed)
{
	unsigned altIdx=0;
	long long  accumulated = 0;
	bool success = true;
	while (instr < lines)
	{
		if (parsed[instr])
		{
			success = false;
			break;
		}
		parsed[instr] = true;
		const instruction& cur = code[instr];
		switch(cur.op)
		{
			case 'a':
				accumulated += cur.val;
				++instr;
				break;
			case 'j':
				if (alt)
					alt[altIdx++] = {instr, accumulated};
				instr += cur.val;
				break;
			case 'n':
				if (alt)
					alt[altIdx++] = {instr, accumulated};
				++instr;
				break;
		}
	}
	return {accumulated, success};
}

long long day8_p2(const instruction* code, unsigned lines, const alternate* alt, bool* parsed)
{
	unsigned instr;
	for (unsigned idx=0; alt[idx].instr!=0; ++idx)
	{
		instr = alt[idx].instr;
		instr += code[instr].op=='j'? 1 : code[instr].val;
		auto [res, success] = day8_p1(code, lines, nullptr, instr, parsed);
		if (success)
			return res + alt[idx].acc;
	}
	return 0;
}

output day08(const char* input, size_t bytes)
{
	Fileio file(input, bytes);
	instruction code[1024]{};
	alternate alt[300]{};
	bool parsed[1024]{};
	unsigned lines;
	LoadInput(file, code, lines);
	auto [res1, success] = day8_p1(code, lines, alt, 0, parsed);
	long long res2 = day8_p2(code, lines, alt, parsed);
	return {res1, res2};
}
