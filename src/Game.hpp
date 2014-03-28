#ifndef GAME_HPP
#define GAME_HPP

#include "Situation.hpp"
#include "compare.hpp"

#include <map>
#include <list>

struct HistoryEntry {
	/// The current position
	Situation situation;
	/// The action that lead to this position
	Action action;
	/// the numerical index of the current position, 0 being the initial pos
	int index;
};

class Game {
public:
	// LIFECYYCLE
	~Game() = default;
	Game(const Game &) = delete;
	Game &operator = (const Game &) = delete;
	Game();
	Game(const Situation &);

	// ACCESS
	const Situation &current_situation() const;
	int position_repetition_counter(const Position &) const;
	int position_repetition_counter() const;

	// OPERATION
	void action(const Action& action);

private:
	/// contains all past positions and corresponding moves
	std::list<HistoryEntry> _history;
	/// track the number of times each position has occured
	std::map<Position, int, PositionCompare> _position_repetition;
};

#endif // GAME_HPP
