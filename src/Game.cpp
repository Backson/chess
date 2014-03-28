#include "Game.hpp"

// LIFECYCLE

Game::Game() {
	const Situation situation;
	const Tile invalid = Board::INVALID_TILE;
	Action action = {PLAYER_NONE, DO_NOTHING, invalid, invalid};
	_history.push_back({situation, action, 0});
}

Game::Game(const Situation &situation) {
	const Tile invalid = Board::INVALID_TILE;
	Action action = {PLAYER_NONE, DO_NOTHING, invalid, invalid};
	_history.push_back({situation, action, 0});
}

// ACCESS

const Situation &Game::current_situation() const {
	return _history.end()->situation;
}

int Game::position_repetition_counter(const Position &position) const {
	auto iter = _position_repetition.find(position);
	if (iter == _position_repetition.end())
		return 0;
	else
		return iter->second;
}

int Game::position_repetition_counter() const {
	return position_repetition_counter(current_situation());
}

// OPERATION

void Game::action(const Action& a) {
	Situation situation = current_situation();
	situation.action(a);
	// add to history
	_history.push_back({situation, a, (int)_history.size()});
	// increment position repetition counter
	++_position_repetition[situation];
}
