#include "common.h"
#include "fileio.h"

struct direction
{
	int x, y;
};

static inline void Rotate(direction& dir1, int rot)
{
	int sn=0, cs=0;
	switch(rot)
	{
	case 90:
		sn = 1, cs=0; break;
	case 180:
		sn=0, cs=-1; break;
	case 270:
		sn=-1, cs=0; break;
	}
	int x = cs*dir1.x - sn*dir1.y;
	int y = sn*dir1.x + cs*dir1.y;
	dir1 = {x,y};
}

output day12(const char* input, size_t bytes)
{
	Fileio file(input, bytes);
	char op;
	int val;
	direction pos1={0,0};
	direction pos2={0,0};
	direction dir1={1,0};
	direction dir2={10,1};
	while (!file.eof())
	{
		op = file.text[file.pos++];
		val = file.ReadPositive<unsigned>();
		switch(op)
		{
		case 'N':
			pos1.y += val; dir2.y += val; break;
		case 'S':
			pos1.y -= val; dir2.y -= val; break;
		case 'E':
			pos1.x += val; dir2.x += val; break;
		case 'W':
			pos1.x -= val; dir2.x -= val; break;
		case 'L':
			Rotate(dir1, val); Rotate(dir2, val); break;
		case 'R':
			Rotate(dir1, -val+360); Rotate(dir2, -val+360); break;
		case 'F':
			pos1.x += val*dir1.x;
			pos1.y += val*dir1.y;
			pos2.x += val*dir2.x;
			pos2.y += val*dir2.y;
			break;
		}
		file.ignorenl();
	}
	return {std::abs(pos1.x)+std::abs(pos1.y),
		      std::abs(pos2.x)+std::abs(pos2.y)};
}
