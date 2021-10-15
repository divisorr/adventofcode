#include "common.h"
#include "fileio.h"


/*Note: The rows and columns containing only "." are skipped from the example.
  Pay attention to the wording of the problem: "The frame of view follows the active cells in each cycle".
  In the example "After 1 cycle" for z=-1 the real plot area should be extended, becase we want to align
  (0,0) from z=0  from cycle 0 and
  (1,1) from z=-1 from cycle 1
  top left is considered (0,0). The expanded area becomes
  .....
  .....
  .#...
  ...#.
  ..#..
*/

	//height=x, width=y, depth=z, spissitude=w
const unsigned height=22, width=22, depth=15, spissitude=15;
const unsigned HW = height*width;
const unsigned HWD = height*width*depth;
const unsigned HWDS = height*width*depth*spissitude;

static inline void RebuildNeighbours3D(const bool* image, unsigned* neighbours, unsigned min, unsigned maxWZ, unsigned maxXY)
{
	for (unsigned z=min; z<=maxWZ; ++z)
		for (unsigned x=min; x<=maxXY; ++x)
			for (unsigned y=min; y<=maxXY; ++y)
				if (image[HW*z + width*x + y] == 1)
				{
					for (unsigned _z=z-1; _z<=z+1; ++_z)
						for (unsigned _x=x-1; _x<=x+1; ++_x)
							for (unsigned _y=y-1; _y<=y+1; ++_y)
								++neighbours[HW*_z + width*_x + _y];
					--neighbours[HW*z + width*x + y];	//Don't increment the current cell
				}
}

static inline void RebuildNeighbours4D(const bool* image, unsigned* neighbours, unsigned min, unsigned maxWZ, unsigned maxXY)
{
	for (unsigned w=min; w<=maxWZ; ++w)
		for (unsigned z=min; z<=maxWZ; ++z)
			for (unsigned x=min; x<=maxXY; ++x)
				for (unsigned y=min; y<=maxXY; ++y)
					if (image[HWD*w + HW*z + width*x + y] == 1)
					{
						for (unsigned _w=w-1; _w<=w+1; ++_w)
							for (unsigned _z=z-1; _z<=z+1; ++_z)
								for (unsigned _x=x-1; _x<=x+1; ++_x)
									for (unsigned _y=y-1; _y<=y+1; ++_y)
										++neighbours[HWD*_w + HW*_z + width*_x + _y];
						--neighbours[HWD*w + HW*z + width*x + y];	//Don't increment the current cell
					}
}

static inline int EvolveCell(bool* cur, const bool* prev, const unsigned* neighbours, unsigned idx)
{
	unsigned count_neighbours = neighbours[idx];
	if (prev[idx] == 1)
	{
		if (count_neighbours<2 || 3<count_neighbours)
		{
			cur[idx] = 0;
			return -1;
		}
	}
	else
	{
		if (count_neighbours == 3)
		{
			cur[idx] = 1;
			return +1;
		}
	}
	return 0;
}

static inline int Solve3D(bool* cur, bool* prev)
{
	int evolved = 0;
	unsigned* neighbours = new unsigned[HWD];
	for (unsigned cycle=1; cycle<=6; ++cycle)
	{
		memcpy(prev, cur, HWD*sizeof(bool));
		memset(neighbours, 0, HWD*sizeof(unsigned));
		unsigned min = 7-cycle;
		unsigned maxWZ = 7+cycle;
		unsigned maxXY = 15+cycle;
		RebuildNeighbours3D(prev, neighbours, min, maxWZ, maxXY);
		for (unsigned z=min; z<=maxWZ; ++z)
		{
			for (unsigned x=min; x<=maxXY; ++x)
			{
				for (unsigned y=min; y<=maxXY; ++y)
				{
					unsigned idx = z*HW + x*width + y;
					evolved += EvolveCell(cur, prev, neighbours, idx);
				}
			}
		}
	}
	delete [] neighbours;
	return evolved;
}

static inline int Solve4D(bool* cur, bool* prev)
{
	unsigned idx;
	int evolved = 0;
	unsigned* neighbours = new unsigned[HWDS]{};
	for (unsigned cycle=1; cycle<=6; ++cycle)
	{
		memcpy(prev, cur, HWDS*sizeof(bool));
		memset(neighbours, 0, HWDS*sizeof(unsigned));
		unsigned min = 7-cycle;
		unsigned maxWZ = 7+cycle;
		unsigned maxXY = 15+cycle;
		RebuildNeighbours4D(prev, neighbours, min, maxWZ, maxXY);
		for (unsigned w=min; w<=maxWZ; ++w)
		{
			for (unsigned z=min; z<=maxWZ; ++z)
			{
				for (unsigned x=min; x<=maxXY; ++x)
				{
					for (unsigned y=min; y<=maxXY; ++y)
					{
						idx = w*HWD + z*HW + x*width + y;
						evolved += EvolveCell(cur, prev, neighbours, idx);
					}
				}
			}
		}
	}
	delete [] neighbours;
	return evolved;
}

output day17(const char* input, size_t bytes)
{
	/*On each cycle the search area expands with 1 in all directions. This means, that the minimum
	size for the arrays is initialsize + 2*6. But since the search is performed with all
	neighbours in 3D/4D, the arrays have to be expanded with 1 more tile in each direction
	because of the tiles around the border. The formula becomes initialsize + 2*6 + 2:
	w=1+12+2; z=1+12+2; x=8+12+2; y=8+12+2
	*/

		//Allocate memory for both parts. Part1 uses small chunk of it
	bool* cur = new bool[HWDS]{};
	bool* prev = new bool[HWDS];

		//Used only for the line terminators
	Fileio file(input, bytes);
	size_t& pos = file.pos;

		//Part 1
	long long part1 = 0;
	bool* plane0 = &cur[HW*7];
	for (unsigned row=7; row<15; ++row)
	{
		for (unsigned col=7; col<15; ++col)
			part1 += plane0[row*width + col] = input[pos++] & 1;
		file.ignorenl();
	}
	part1 += Solve3D(cur, prev);

		//Part 2
	pos = 0;
	long long part2 = 0;
	memset(cur, 0, HWD*sizeof(bool));
	plane0 = &cur[HWD*7 + HW*7];
	for (unsigned row=7; row<15; ++row)
	{
		for (unsigned col=7; col<15; ++col)
			part2 += plane0[row*width + col] = input[pos++] & 1;
		file.ignorenl();
	}
	part2 += Solve4D(cur, prev);

	delete [] prev;
	delete [] cur;

	return {part1,part2};
}
