#include "common.h"
#include "fileio.h"

output day24(const char* pFile, size_t bytes)
{
	Fileio file(pFile, bytes);

		//The array for part1 is small.
		//The array for part2 can be increased on each step by at most 1 tile in each direction.
		//All tiles are initialized with 0 as white.
	const unsigned tileSize = 150;
	const unsigned center = tileSize/2;
	bool *tiles = new bool[tileSize*tileSize]{};
		//q == x; r == z in Cube coordinates.
		//x+y+z=0, so y can be derived knowing x and z. These are called Axial coordinates
	int q,r;
	int minQ, maxQ;
	int minR, maxR;
	minQ = maxQ = minR = maxR = center;
	unsigned p1_res=0;
		//Initialization and solution for part1
	while (!file.eof())
	{
			//Start from the center
		q = r = center;
		while (file.text[file.pos] > ' ')	//The last row of letters doesn't have newline at the end
		{
			switch(file.text[file.pos++])
			{
				case 'e':
					++q;
					break;
				case 'w':
					--q;
					break;
				case 'n':
					file.text[file.pos++] == 'e'? (++q,--r) : --r;
					break;
				case 's':
					file.text[file.pos++] == 'e'? ++r: (--q,++r);
					break;
			}
		}
		file.ignorenl();
		tiles[q*tileSize + r] = !tiles[q*tileSize + r];
		tiles[q*tileSize + r]? ++p1_res : --p1_res;

		if (q < minQ)
			minQ = q;
		else if (q > maxQ)
			maxQ = q;

		if (r < minR)
			minR = r;
		else if (r > maxR)
			maxR = r;
	}

		//Part2 solution
	unsigned p2_res = p1_res;
		//Array with neighbors
	bool *neighbors = new bool[tileSize*tileSize];

	--minQ, ++maxQ;
	--minR, ++maxR;
	for (unsigned round=1; round<=100; ++round)
	{
		int minX = minQ;
		int maxX = maxQ;
		int minZ = minR;
		int maxZ = maxR;
		memcpy(neighbors, tiles, tileSize*tileSize*sizeof(bool));
		for (int x=minX; x<=maxX; ++x)
		{
			for (int z=minZ; z<=maxZ; ++z)
			{
				unsigned numBlack = 0;
				numBlack += neighbors[x*tileSize + z-1];
				numBlack += neighbors[x*tileSize + z+1];
				numBlack += neighbors[(x-1)*tileSize + z];
				numBlack += neighbors[(x+1)*tileSize + z];
				numBlack += neighbors[(x-1)*tileSize + z+1];
				numBlack += neighbors[(x+1)*tileSize + z-1];
				if (neighbors[x*tileSize + z])	//black tile?
				{
					if (numBlack==0 || numBlack>2)
					{
						tiles[x*tileSize + z] = false; //flip to white
						--p2_res;
					}
				}
				else
				{
					if (numBlack == 2)
					{
						tiles[x*tileSize + z] = true; //flip to black
						++p2_res;
						minQ -= x==minQ;
						maxQ += x==maxQ;
						minR -= z==minR;
						maxR += z==maxR;
					}
				}
			}
		}
	}

	delete [] neighbors;
	delete [] tiles;
	return {p1_res,p2_res};
}
