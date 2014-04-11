#include "MoveCache.hpp"

#include "Rules.hpp"

#include <memory>

using std::move;

void MoveCache::clear() {
	_cache.clear();
}

const MoveCache::ActionVector &MoveCache::legal_moves(const Position &pos) {
    Rules rules;

	auto iter = _cache.find(pos);
	if (iter == _cache.end()) {
		ActionVector av;
		rules.getAllLegalMoves(pos, av);
		auto result = _cache.emplace(pos, av);

		return result.first->second;
	} else {
		return iter->second;
	}
}

MoveCache MoveCache::global;
