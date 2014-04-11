#ifndef MOVE_CACHE_HPP
#define MOVE_CACHE_HPP

#include "Position.hpp"
#include "Action.hpp"
#include "compare.hpp"

#include <vector>
#include <map>

class MoveCache {
public:

	typedef std::vector<Action> ActionVector;

	MoveCache() = default;
	~MoveCache() = default;
	MoveCache(const MoveCache &) = delete;
	MoveCache &operator = (const MoveCache &) = delete;

	void clear();
	const ActionVector &legal_moves(const Position &);

	static MoveCache global;

private:
	std::map<Position, ActionVector, PositionCompare> _cache;
};

#endif // MOVE_CACHE_HPP
