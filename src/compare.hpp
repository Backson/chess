#ifndef COMPARE_HPP
#define COMPARE_HPP

#include "Position.hpp"

struct BoardCompare {
	bool operator () (const Board &, const Board &) const;
};

struct PositionCompare {
	bool operator () (const Position &, const Position &) const;
};

struct BoardHash {
	uint32 operator () (const Board &) const;
};

struct PositionHash {
	uint32 operator () (const Position &) const;
};

#endif // COMPARE_HPP
