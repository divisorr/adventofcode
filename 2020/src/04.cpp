#include "common.h"
#include "fileio.h"

	//Perfect hash function for fields in range [0,8]. Found using bruteforce
static inline constexpr unsigned field_hasher(const char* in)
{
	return ((in[0]<<16) + (in[1]<<8) + in[2]) % 477 % 8;
}

	//This is not always inlined and field_hasher() gets slower if it calls to_unsigned()
static inline constexpr unsigned to_unsigned(const char* in)
{
	return ((in[0]<<16) + (in[1]<<8) + in[2]);
}

	//Storage for the hashed values
enum fields
{
	byr = field_hasher("byr"),
	iyr = field_hasher("iyr"),
	eyr = field_hasher("eyr"),
	hgt = field_hasher("hgt"),
	hcl = field_hasher("hcl"),
	ecl = field_hasher("ecl"),
	pid = field_hasher("pid"),
	cid = field_hasher("cid")
};

	//Array for eye color with values converted from char* to unsigned.
	//Ordered by hash value obtained from the hash function in ValidEyeColor()
static const unsigned eye_colors[7] =
{
	to_unsigned("grn"),
	to_unsigned("hzl"),
	to_unsigned("amb"),
	to_unsigned("blu"),
	to_unsigned("gry"),
	to_unsigned("brn"),
	to_unsigned("oth")
};

static inline bool ValidByr(const char* input, size_t)
{
	unsigned year = (unsigned)atoi(input);
	return 1920<=year && year<=2002;
}

static inline bool ValidIyr(const char* input, size_t)
{
	unsigned year = (unsigned)atoi(input);
	return 2010<=year && year<=2020;
}

static inline bool ValidEyr(const char* input, size_t)
{
	unsigned year = (unsigned)atoi(input);
	return 2020<=year && year<=2030;
}

	//Read positive number from string
static inline unsigned ReadPositive(const char* string)
{
	unsigned res = 0;
	for (unsigned idx=0; '0'<=string[idx] && string[idx]<='9'; )
		res = res*10 + (string[idx++]-'0');
	return res;
}

static inline bool ValidHeight(const char* input, size_t len)
{
	unsigned height = ReadPositive(input);
	if (input[len-1] == 'm')
		return 150<=height && height<=193;
	else
		return 59<=height && height<=76;
}

static inline bool ValidHairColor(const char* input, size_t len)
{
	if (len!=7)
		return false;
	if (input[0]!='#')
		return false;
	for (size_t idx=1; idx<len; ++idx)
	{
			//Splitting the condition in two if statements has higher chance to return sooner
		if(input[idx]<'0' || 'f'<input[idx])
			return false;
		if('9'<input[idx] && input[idx]<'a')
			return false;
	}
	return  true;
}

static inline bool ValidEyeColor(const char* input, size_t)
{
	unsigned ecl = to_unsigned(input);
		//Perfect hash function for eye colors in range [0,7]. Found using bruteforce
	return (ecl == eye_colors[ecl % 122 % 7]);
}

static inline bool ValidPid(const char*, size_t len)
{
	return len==9;
}

static inline bool ValidCid(const char*, size_t)
{
	return true;
}

output day04(const char* input, size_t bytes)
{
	long long part1=0, part2=0;
	Fileio file(input, bytes);
	size_t& pos = file.pos;

		//Pointers to validate functions ordered by increasing hash value.
	bool (*Validate[])(const char*, size_t) =
	{
		ValidPid,
		ValidEyr,
		ValidCid,
		ValidIyr,
		ValidHeight,
		ValidByr,
		ValidHairColor,
		ValidEyeColor
	};

		//Cid is always valid
	unsigned char valid_p1 = 1<<fields::cid;
	unsigned char valid_p2 = 1<<fields::cid;
	size_t beg,size;
	while (!file.eof())
	{
		unsigned field_hash = field_hasher(input+pos);
		beg = pos += 4;
		size = file.AdvanceString();
		valid_p1 |= 1<<field_hash;
		valid_p2 |= Validate[field_hash](input+beg, size) << field_hash;

		if (input[pos] < ' ')
		{
			file.ignorenl();
				//for the last line pos>bytes and input[pos] is mem access violation
			if (pos>bytes || input[pos] < ' ')
			{
				file.ignorenl();
					//End of current passport.
				part1 += (valid_p1 == 255);
				part2 += (valid_p2 == 255);
				valid_p1 = valid_p2 = 1<<fields::cid;
			}
		}
		else
			file.ignore();
	}

	return {part1, part2};
}
