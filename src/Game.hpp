#ifndef GAME_HPP
#define GAME_HPP

#include "Position.hpp"
#include "compare.hpp"

#include <map>
#include <list>

struct PositionListEntry {
	/// The current position
	Position position;
	/// The action that lead to this position
	Action action;
	/// the numerical index of the current position, 0 being the initial pos
	int index;
};

class Game {
public:
	Game();
	Game(const Position &);
	Game(const Game &) = delete;
	~Game() = default;
	Game &operator = (const Game &) = delete;
	
	const Position &current_position() const;
	int position_repetition_counter(const Position &) const;
	int position_repetition_counter() const;
	int half_turn_counter();
	
	void action(const Action& action);
	
private:
	/// contains all past positions and corresponding moves
	std::list<PositionListEntry> _history;
	/// track the number of times each position has occured
	std::map<Position, int, PositionCompare> _position_repetition;
	/// how many half turns without a capture or pawn move
	int _half_turn_counter;
};

#endif // GAME_HPP
