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

	typedef std::list<HistoryEntry>::iterator HistoryIter;
	typedef std::list<HistoryEntry>::const_iterator HistoryConstIter;

	// LIFECYYCLE
	Game(const Game &) = delete;
	Game &operator = (const Game &) = delete;
	Game();
	Game(const Situation &);

	// ACCESS
	const Situation &current_situation() const;
	int position_repetition_counter(const Position &) const;
	int position_repetition_counter() const;

	const std::list<HistoryEntry> &history() const;

	// OPERATION
	void action(const Action &action);
	void seek(int number);
	void pop();

	void reset();
	void reset(const Situation &);

private:
	/// contains all past positions and corresponding moves
	std::list<HistoryEntry> _history;
	/// track the number of times each position has occured
	std::map<Position, int, PositionCompare> _position_repetition;
};

#endif // GAME_HPP
