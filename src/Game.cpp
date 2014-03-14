#include "Game.hpp"

Game::Game() {
	const Position position;
	const Tile invalid = Board::INVALID_TILE;
	Action action = {PLAYER_NONE, DO_NOTHING, invalid, invalid, TYPE_NONE};
	_history.push_back({position, action, 0});
}

Game::Game(const Position &position) {
	const Tile invalid = Board::INVALID_TILE;
	Action action = {PLAYER_NONE, DO_NOTHING, invalid, invalid, TYPE_NONE};
	_history.push_back({position, action, 0});
}

const Position &Game::current_position() const {
	return _history.end()->position;
}

int Game::position_repetition_counter(const Position &position) const {
	auto iter = _position_repetition.find(position);
	if (iter == _position_repetition.end())
		return 0;
	else
		return iter->second;
}

int Game::position_repetition_counter() const {
	return position_repetition_counter(current_position());
}

int Game::half_turn_counter() {
	return _half_turn_counter;
}

void action(const Action& action) {
	
}