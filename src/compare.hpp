#ifndef COMPARE_HPP
#define COMPARE_HPP

#include "Position.hpp"

struct BoardCompare {
	bool operator () (const Board &, const Board &) const;
};

struct PositionCompare {
	bool operator () (const Position &, const Position &) const;
};

#endif // COMPARE_HPP
