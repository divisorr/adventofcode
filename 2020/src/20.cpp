#include "common.h"
#include "fileio.h"

//Reverse the lower 10bits.
static inline unsigned Reverse10(unsigned number)
{
	number = (number & 0xaaaa)>>1 | (number & 0x5555)<<1;
	number = (number & 0xcccc)>>2 | (number & 0x3333)<<2;
	number = (number & 0xf0f0)>>4 | (number & 0x0f0f)<<4;
	number = (number & 0xff00)>>8 | (number & 0x00ff)<<8;
	return number>>6;
}

static inline uint64_t MirrorV64(uint64_t number)
{
	number = ((number >>  8) & 0x00FF00FF00FF00FF) | ((number & 0x00FF00FF00FF00FF) <<  8);
	number = ((number >> 16) & 0x0000FFFF0000FFFF) | ((number & 0x0000FFFF0000FFFF) << 16);
	number = (number >> 32) | (number<< 32);
	return number;
}

static inline uint64_t MirrorH64(uint64_t number)
{
	number = ((number >> 1) & 0x5555555555555555) +  2*(number & 0x5555555555555555);
	number = ((number >> 2) & 0x3333333333333333) +  4*(number & 0x3333333333333333);
	number = ((number >> 4) & 0x0f0f0f0f0f0f0f0f) + 16*(number & 0x0f0f0f0f0f0f0f0f);
	return number;
}

static inline uint64_t FlipDiag(uint64_t x)
{
	uint64_t t = 0x0f0f0f0f00000000 & (x ^ (x << 28));
	x ^= t ^ (t >> 28) ;
	t = 0x3333000033330000 & (x ^ (x << 14));
	x ^= t ^ (t >> 14) ;
	t = 0x5500550055005500 & (x ^ (x <<  7));
	x ^= t ^ (t >>  7) ;
	return x;
}

static inline uint64_t RotateCCW(uint64_t x)
{
	return MirrorV64(FlipDiag(x));
}

static inline uint64_t RotateCW(uint64_t x)
{
	return FlipDiag(MirrorV64(x));
}

/*Each tile is a square with side=10.
Each side of a tile has exactly one other matching side in another tile. Border sides don't have a match of course.*/
struct Tile
{
public:
	enum side
	{
		TOP    = 0,
		LEFT   = 1,
		BOTTOM = 2,
		RIGHT  = 3
	};
	unsigned sides[4];
		//Core is 8x8 bits.
	uint64_t core = 0;
	unsigned id=0;

public:
	void Read(Fileio& file);
	void Flip(Tile::side sideToReverse);

private:
	static unsigned FetchLine(Fileio& file);
};

//static
inline unsigned Tile::FetchLine(Fileio& file)
{
	unsigned res = 0;
	for (int bit=9; bit>=0; --bit)
		res |= (file.text[file.pos++]&1) << bit;
	file.ignorenl();
	return res;
}

inline void Tile::Read(Fileio& file)
{
	file.ignore(5); //"Tile "
	id = file.ReadPositive<unsigned>();
	file.ignore(); //":"
	file.ignorenl();

	unsigned& top = sides[Tile::TOP];
	unsigned& left = sides[Tile::LEFT];
	unsigned& right = sides[Tile::RIGHT];
	unsigned& bottom = sides[Tile::BOTTOM];

		//Top
	unsigned line = FetchLine(file);
	top = line;
	left = line & 512; //The 10th bit is index 2^9
	right = (line & 1) << 9;  //The 0th bit
		//left, core, right
	for (int idx=1; idx<9; ++idx)
	{
		line = FetchLine(file);
		left |= (line & 512) >> idx;
		core |= (uint64_t)((line & 510)>>1) << 8*(8-idx); //510 has 1 bits in positions [1,8].
		right |= (line & 1) << (9-idx);
	}
		//Bottom
	line = FetchLine(file);
	bottom = line;
	left |= (line & 512) >> 9;
	right |= line & 1;
}

void Tile::Flip(Tile::side sideToReverse)
{
	if (sideToReverse==Tile::side::LEFT || sideToReverse==Tile::side::RIGHT)
	{	//Row 1->n, 2->n-1, ...
		std::swap(sides[TOP], sides[BOTTOM]);
		sides[LEFT] = Reverse10(sides[LEFT]);
		sides[RIGHT] = Reverse10(sides[RIGHT]);
		core = MirrorV64(core);
	}
	else
	{	//Column 1->n, 2->n-1, ...
		std::swap(sides[LEFT], sides[RIGHT]);
		sides[TOP] = Reverse10(sides[TOP]);
		sides[BOTTOM] = Reverse10(sides[BOTTOM]);
		core = MirrorH64(core);
	}
}

struct Solver20
{
public:
	Solver20(const char* input, size_t bytes);
	long long ArrangePuzzle(); //solves part1
	long long DetermineRoughness(); //solves part2

private:
		//The initial grid 12x12. Owner of the memory for all tiles.
	Tile tiles[144];
	struct CachePair{Tile* tile; Tile::side side;};
		//For faster lookup each side puts a reference in this array to the tile it belongs.
	CachePair cache[1024][2]{}; //Each side consists of 10 bits. At most 2 sides are allowed to have the same value
		//The ordered result. [0][0] is top left; [11][11] is bottom right.
	Tile* ordered[12][12];
		//The core of the ordered result. Each cell of the core can already be found in each tile.
	uint64_t core[12][12];
		//Used to determine the axis of mirror when a tile needs to be flipped.
	enum class AXIS
	{
		NO, V, H
	};
		//Number of 1 bits in all tiles' cores
	unsigned roughness;

private:
		//Get a matching tile who's side matches the side of the fixed tile.
		//Transforms the matching tile (by rotation, mirror etc.) in the process if needed.
		//Returns nullptr if the side doesn't have a match - that side is a border
	Tile* MatchSide(const Tile* srcTile, Tile::side srcSide);
		//Transforms the tile by number of PI/2 rotations. Negative number is CCw, positive is CW, 0 - no transformations.
	void TransformTile(Tile* toTransform, int rotation);
	void FlipTile(Tile* toTransform, Tile::side sideToReverse);
		//Cache functions
	CachePair* GetPairFromCache(unsigned side, const Tile* srcTile);
	void AddAllSidesToCache(Tile* tile);
	void AddOneSide(Tile* tile, Tile::side tileSide);
	void PurgeAllSidesFromCache(const Tile* tile);
	bool PurgeOneSide(const Tile* tile, Tile::side tileSide);
		//Check if the tile needs flip because a side won't match the fixed side after rotation.
	Solver20::AXIS DetermineFlipAxis(Tile::side curSide, Tile::side fixedSide);
		//Part2 functions
	unsigned CountSeaMonsters();
	void BuildCore(uint64_t out[][2]);
		//Mirror the core of the entire grid along the Y axis
	void CoreMirrorH();
	void CoreMirrorDiag();
};

Solver20::Solver20(const char* input, size_t bytes)
{
	roughness = 0;
	Fileio file(input, bytes);
	for (unsigned idx=0; idx<144; ++idx)
	{
		tiles[idx].Read(file);
		file.ignorenl();	//Separating \n between tiles
			//Cache it
		AddAllSidesToCache(&tiles[idx]);
		roughness += std::popcount(tiles[idx].core);
	}
}

long long Solver20::ArrangePuzzle()
{
		//Choose one tile. From it find the tile in the top left corner of the grid
	Tile* matchingTile = &tiles[0];
	Tile* curTile = nullptr;
		//Move up
	while(matchingTile)
	{
		curTile = matchingTile;
		matchingTile = MatchSide(matchingTile, Tile::TOP);
	}
	matchingTile = curTile;
		//Move left
	while(matchingTile)
	{
		curTile = matchingTile;
		matchingTile = MatchSide(matchingTile, Tile::LEFT);
	}
		//Start ordering and transforming tiles from top-left to bottom-right
	for (unsigned row=0; curTile; ++row)
	{
		for (unsigned col=0; curTile; ++col)
		{
			core[row][col] = curTile->core;
			ordered[row][col] = curTile;
			PurgeAllSidesFromCache(curTile);
			curTile = MatchSide(curTile, Tile::RIGHT);
		}
		curTile = MatchSide(ordered[row][0], Tile::BOTTOM);
	}

	return (long long)ordered[0][0]->id*ordered[0][11]->id*ordered[11][0]->id*ordered[11][11]->id;
}

Tile* Solver20::MatchSide(const Tile* srcTile, Tile::side srcSide)
{
	unsigned side = srcTile->sides[srcSide];
	CachePair* match = GetPairFromCache(side, srcTile);
	Tile::side curSide;
		//match is a reference from the cache. If the cache entry gets updated match becomes invalid.
		//Instead work directly on the tile
	Tile* curTile;
	if (match)
	{ //Tile found, it's face correctly pointing up.
		curTile = match->tile;
		curSide = match->side;
	}
	else
	{
		side = Reverse10(side);
		match = GetPairFromCache(side, srcTile);
		if (match)
		{ //Tile found but it is face down. Turn it up.
			curTile = match->tile;
			curSide = match->side;
			FlipTile(curTile, curSide);
		}
		else
			return nullptr;
	}
		//Check if the current side can be transformed to the target side without flip of the tile. Target side is always the opposite of the srcSide.
	AXIS flip = DetermineFlipAxis((Tile::side)curSide, (Tile::side)((srcSide+2)%4));
	if (flip != AXIS::NO)
		FlipTile(curTile, (flip==AXIS::H)? Tile::side::LEFT : Tile::side::TOP);	//LEFT and TOP are just placeholders for Horizontal or Vertical
		//Now find the wanted rotation
	int rotation = (srcSide+2 - curSide)%4;	//+2 because we want the opposite side of the source side
	TransformTile(curTile, rotation);
	return curTile;
}

Solver20::CachePair* Solver20::GetPairFromCache(unsigned side, const Tile* srcTile)
{
	CachePair* pair = cache[side];
	if(pair[0].tile && pair[0].tile!=srcTile)
		return &pair[0];
	else if (pair[1].tile && pair[1].tile!=srcTile)
		return &pair[1];
	else
		return nullptr;
}

static inline void RotateSides(unsigned* sides, bool bCCW)
{
	unsigned temp = sides[0];
	if (bCCW)
	{
		sides[0] = sides[3];
		sides[3] = sides[2];
		sides[2] = sides[1];
		sides[1] = temp;
	}
	else
	{
		sides[0] = sides[1];
		sides[1] = sides[2];
		sides[2] = sides[3];
		sides[3] = temp;
	}
}

void Solver20::TransformTile(Tile* tile, int rotation)
{
	if (rotation == 0)
		return;
	if (rotation == 3)
		rotation = -1;
	else if (rotation == -3)
		rotation = 1;
		//Purge the cache from all sides of the tile
	PurgeAllSidesFromCache(tile);

	if (rotation == 1)
	{  //Rotate CCW
		tile->sides[Tile::side::TOP] = Reverse10(tile->sides[Tile::side::TOP]);
		tile->sides[Tile::side::BOTTOM] = Reverse10(tile->sides[Tile::side::BOTTOM]);
		RotateSides(tile->sides, true);
		tile->core = RotateCCW(tile->core);
	}
	else if (rotation == -1)
	{  //Rotate CW
		tile->sides[Tile::side::RIGHT] = Reverse10(tile->sides[Tile::side::RIGHT]);
		tile->sides[Tile::side::LEFT] = Reverse10(tile->sides[Tile::side::LEFT]);
		RotateSides(tile->sides, false);
		tile->core = RotateCW(tile->core);
	}
	else
	{	//rotation of 2/-2
		unsigned* side = tile->sides;
		std::swap(side[Tile::side::LEFT], side[Tile::side::RIGHT]);
		side[Tile::side::TOP] = Reverse10(side[Tile::side::TOP]);
		side[Tile::side::BOTTOM] = Reverse10(side[Tile::side::BOTTOM]);
		tile->core = MirrorH64(tile->core);
		std::swap(side[Tile::side::TOP], side[Tile::side::BOTTOM]);
		side[Tile::side::LEFT] = Reverse10(side[Tile::side::LEFT]);
		side[Tile::side::RIGHT] = Reverse10(side[Tile::side::RIGHT]);
		tile->core = MirrorV64(tile->core);
	}
		//Update the cache
	AddAllSidesToCache(tile);
}

void Solver20::FlipTile(Tile* toTransform, Tile::side sideToReverse)
{
	PurgeAllSidesFromCache(toTransform);
	toTransform->Flip(sideToReverse);
	AddAllSidesToCache(toTransform);
}

void Solver20::AddAllSidesToCache(Tile* tile)
{
	AddOneSide(tile, Tile::side::TOP);
	AddOneSide(tile, Tile::side::LEFT);
	AddOneSide(tile, Tile::side::RIGHT);
	AddOneSide(tile, Tile::side::BOTTOM);
}

void Solver20::AddOneSide(Tile* tile, Tile::side tileSide)
{
	CachePair* pair = cache[tile->sides[tileSide]];
	if (pair[0].tile == nullptr)
	{
		pair[0].tile = tile;
		pair[0].side = tileSide;
	}
	else
	{
		pair[1].tile = tile;
		pair[1].side = tileSide;
	}
}

void Solver20::PurgeAllSidesFromCache(const Tile* tile)
{
	PurgeOneSide(tile, Tile::side::TOP);
	PurgeOneSide(tile, Tile::side::BOTTOM);
	PurgeOneSide(tile, Tile::side::LEFT);
	PurgeOneSide(tile, Tile::side::RIGHT);
}

bool Solver20::PurgeOneSide(const Tile* tile, Tile::side side)
{
	CachePair* pair = cache[tile->sides[side]];
	if (pair[0].tile == tile)
	{
		pair[0].tile = nullptr;
		return true;
	}
	else if (pair[1].tile == tile)
	{
		pair[1].tile = nullptr;
		return true;
	}
	return false;
}

Solver20::AXIS Solver20::DetermineFlipAxis(Tile::side curSide, Tile::side targetSide)
{
	if (curSide == targetSide)
		return Solver20::AXIS::NO;

	if (curSide == Tile::side::TOP)
	{
		if (targetSide == Tile::side::RIGHT)
			return Solver20::AXIS::NO;
		else
			return Solver20::AXIS::V;
	}

	if (curSide == Tile::side::RIGHT)
	{
		if (targetSide == Tile::side::TOP)
			return Solver20::AXIS::NO;
		else
			return Solver20::AXIS::H;
	}

	if (curSide == Tile::side::BOTTOM)
	{
		if (targetSide == Tile::side::LEFT)
			return Solver20::AXIS::NO;
		else
			return Solver20::AXIS::V;
	}

	if (curSide == Tile::side::LEFT)
	{
		if (targetSide == Tile::side::BOTTOM)
			return Solver20::AXIS::NO;
		else
			return Solver20::AXIS::H;
	}
		//Unreachable
	return Solver20::AXIS::NO;
}

	//Consecutevly apply one of 8 transformations before finding the correct orientation
long long Solver20::DetermineRoughness()
{
	unsigned numMonsters;
	for (unsigned idx=0; idx<4; ++idx)
	{
		numMonsters = CountSeaMonsters();
		if (numMonsters)
			break;
		CoreMirrorH();
		numMonsters = CountSeaMonsters();
		if (numMonsters)
			break;
		CoreMirrorDiag();	//Rotate is less efficient than flip around diagonal.
	}

	return roughness-(numMonsters*15);
}

unsigned Solver20::CountSeaMonsters()
{
	unsigned count = 0;
	uint64_t work[96][2]{};
	BuildCore(work);
	for (unsigned idx=1; idx<95; ++idx)
	{
		uint64_t num = work[idx][0]; //First part of the row with 64 bits
		for (unsigned offset=0; offset<45; ++offset)
		{
			if (((num & ((uint64_t)549255<<offset))>>offset) == 549255)	//549255 == 0b10000110000110000111
			{
				if (((work[idx+1][0] & ((uint64_t)299592<<offset))>>offset) == 299592) //299592 == 0b1001001001001001000
				{
					if (((work[idx-1][0] & ((uint64_t)2<<offset)) >> offset) == 2) //2 == 0b10
					{
						++count;
					}
				}
			}
		}

		num = ((work[idx][0] & 0x7ffff) << 32) | work[idx][1]; //Second part of the row with 51 bits (0x7ffff is 19 bits long)
		for (unsigned offset=0; offset<32; ++offset)
		{
			if (((num & ((uint64_t)549255<<offset))>>offset) == 549255)	//549255 == 0b10000110000110000111
			{
				uint64_t num2 = ((work[idx+1][0] & 0x7ffff) << 32) | work[idx+1][1];
				if (((num2 & ((uint64_t)299592<<offset))>>offset) == 299592) //299592 == 0b1001001001001001000
				{
					uint64_t num3 = ((work[idx-1][0] & 0x7ffff) << 32) | work[idx-1][1];
					if (((num3 & ((uint64_t)2<<offset)) >> offset) == 2) //2 == 0b10
					{
						++count;
					}
				}
			}
		}
	}
	return count;
}

void Solver20::BuildCore(uint64_t out[][2])
{
	for (unsigned r=0; r<12; ++r)
	{
		for (unsigned c=0; c<8; ++c)
		{
			uint64_t num = core[r][c];
			for (unsigned idx=0; idx<8; ++idx)
			{
				out[r*8 + 7-idx][0] |= ((num & ((uint64_t)0xFF << idx*8))>>idx*8) << (56-c*8);
			}
		}

		for (unsigned c=0; c<4; ++c)
		{
			uint64_t num = core[r][c+8];
			for (unsigned idx=0; idx<8; ++idx)
			{
				out[r*8 + 7-idx][1] |= ((((num & ((uint64_t)0xFF << idx*8))>>idx*8) << (56-c*8)))>>32;
			}
		}
	}
}

void Solver20::CoreMirrorH()
{
	for (unsigned r=0; r<12; ++r)
	{
		uint64_t* row = core[r];
		for (unsigned c=0; c<12; c+=2)
		{
			row[c] = MirrorH64(row[c]);
			row[11-c] = MirrorH64(row[11-c]);
			std::swap(row[c], row[11-c]);
		}
	}
}

void Solver20::CoreMirrorDiag()
{
	for (unsigned r=0; r<12; ++r)
	{
		core[r][r] = FlipDiag(core[r][r]); //the tiles from main diag keep their place
		for (int c=r+1; c<12; ++c)
		{
			core[r][c] = FlipDiag(core[r][c]);
			core[c][r] = FlipDiag(core[c][r]);
			std::swap(core[r][c], core[c][r]);
		}
	}
}

output day20(const char* input, size_t bytes)
{
	Solver20 s(input, bytes);
	long long part1 = s.ArrangePuzzle();
	long long part2 = s.DetermineRoughness();
	return {part1,part2};
}