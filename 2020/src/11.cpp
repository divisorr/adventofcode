#include "common.h"
#include "fileio.h"

#include <list>


struct Neighbours
{
	size_t addresses[8];
	unsigned occupied;
};

	//Find the direct neighbours of each seat
static inline void BuildNeighboursList1(const char* seats, size_t rows, size_t columns, Neighbours* neighbours, std::list<unsigned>& possibleSeats)
{
	const int directions[8][2] = {{-1,-1},{0,-1},{1,-1}, {-1,0},{1,0}, {-1,1},{0,1},{1,1}};
	int dx, dy;
	size_t x, y;
	for (int row=0; row<(int)rows; ++row)
	{
		for (int column=0; column<(int)columns; ++column)
		{
			size_t idx = row*columns+column;
			if (seats[idx] == 'L')
			{
				possibleSeats.push_back((unsigned)idx);
				size_t* addresses = neighbours[idx].addresses;
				unsigned& occupied = neighbours[idx].occupied;
				for (auto& dir : directions)
				{
					dx=dir[0], dy=dir[1];
					x=row+dx,y=column+dy;
					if((0<=x && x<rows) && (0<=y && y<columns))
					{
						if (seats[x*columns+y] == 'L')
							addresses[occupied++] = x*columns+y;
					}
				}
			}
		}
	}
}

	//There are floor tiles which should be skipped and the search for neighbours should continue until a seat is reached.
	//It's enough to stop on the first neigbour found.
static inline void BuildNeighboursList2(const char* seats, size_t rows, size_t columns, Neighbours* neighbours, std::list<unsigned>& possibleSeats)
{
	const int directions[8][2] = {{-1,-1},{0,-1},{1,-1}, {-1,0},{1,0}, {-1,1},{0,1},{1,1}};
	int dx, dy;
	size_t x, y;
	for (size_t row=0; row<rows; ++row)
	{
		for (size_t column=0; column<columns; ++column)
		{
			size_t idx = row*columns+column;
			if (seats[idx] == 'L')
			{
				possibleSeats.push_back((unsigned)idx);
				size_t* addresses = neighbours[idx].addresses;
				unsigned& occupied = neighbours[idx].occupied;
				for (auto& dir : directions)
				{
					dx=dir[0], dy=dir[1];
					for (x=row+dx,y=column+dy; (0<=x && x<rows) && (0<=y && y<columns); x+=dx,y+=dy)
					{
						if (seats[x*columns+y] == 'L')
						{
							addresses[occupied++] = x*columns+y;
							break;
						}
					}
				}
			}
		}
	}
}

static inline unsigned OccupiedNeighbours(const char* seats, unsigned pos, const Neighbours* neighbours)
{
	const size_t* addesses = neighbours[pos].addresses;
	unsigned numNeighbours = neighbours[pos].occupied;
	unsigned occupied = 0;
	for (unsigned idx=0; idx<numNeighbours; ++idx)
		occupied += seats[addesses[idx]]=='#';
	return occupied;
}

static size_t solve(char* cur, size_t size, const Neighbours* neighbours, std::list<unsigned>& possibleSeats, unsigned maxVisibleSeats)
{
	char* prev = new char[size];
	size_t taken = 0;
	unsigned curSeat, takenNeighbours;
	std::list<unsigned>::iterator it, prevIt;
	while (possibleSeats.size())
	{
		memcpy(prev, cur, size);
		for (it=possibleSeats.begin(); it!=possibleSeats.end(); )
		{
			curSeat = *it;
			takenNeighbours = OccupiedNeighbours(prev, curSeat, neighbours);
			if (prev[curSeat] == '#')
			{
				if (takenNeighbours >= maxVisibleSeats)
				{
					cur[curSeat] = 'L';
					--taken;
					++it;
				}
				else
				{
					prevIt = it;
					++it;
					possibleSeats.erase(prevIt);
				}
			}
			else
			{
				if (takenNeighbours == 0)
				{
					cur[curSeat] = '#';
					++taken;
					++it;
				}
				else
				{
					prevIt = it;
					++it;
					possibleSeats.erase(prevIt);
				}
			}
		}
	}
	delete [] prev;
	return taken;
}

output day11(const char* input, size_t bytes)
{
	Fileio file(input, bytes);
	char nl = file.nl;
	size_t columns;
	for (columns=0; input[columns]!=nl; ++columns);
		//Each row has a line terminator, which is counted towards the row length
		//The last row doesnt't have \n at the end, but the calculation has to count it.
	size_t rows = (file.size+file.nlSize)/(columns+file.nlSize);

	char* cur = new char[rows*columns];
	long long part1, part2;
		//Filter all newline characters
	size_t columnnl = columns+file.nlSize;
	for (unsigned row=0; row<rows; ++row)
		memcpy(cur+row*columns, input+row*columnnl, columns);
	std::list<unsigned> possibleSeats;
	Neighbours* neighbours = new Neighbours[rows*columns]{};
	BuildNeighboursList1(cur, rows, columns, neighbours, possibleSeats);
	part1 = solve(cur, rows*columns, neighbours, possibleSeats, 4);

	for (unsigned row=0; row<rows; ++row)
		memcpy(cur+row*columns, input+row*columnnl, columns);
	memset(neighbours, 0, rows*columns*sizeof(Neighbours));
	BuildNeighboursList2(cur, rows, columns, neighbours, possibleSeats);
	part2 = solve(cur, rows*columns, neighbours, possibleSeats, 5);

	delete [] neighbours;
	delete [] cur;

	return {part1,part2};
}
