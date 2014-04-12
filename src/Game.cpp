#include "Game.hpp"

// LIFECYCLE

Game::Game() {
	reset();
}

Game::Game(const Situation &situation) {
	reset(situation);
}

// ACCESS

const Situation &Game::current_situation() const {
	return _history.back().situation;
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

const std::list<HistoryEntry> &Game::history() const {
	return _history;
}

// OPERATION

void Game::action(const Action &a, Delta *delta) {
	Situation situation = current_situation();
	situation.action(a, delta);
	// add to history
	_history.push_back({situation, a, (int)_history.size()});
	// increment position repetition counter
	++_position_repetition[situation];
}

void Game::seek(int number) {
	int current = _history.back().index;
	for (int i = current; i > number; --i)
		pop();
}

void Game::pop() {
	--_position_repetition[current_situation()];
	_history.pop_back();
}

void Game::reset() {
	const Situation situation;
	reset(situation);
}

void Game::reset(const Situation &situation) {
	_history.clear();
	_position_repetition.clear();
	const Tile invalid(situation.INVALID_TILE);
	Action action = {PLAYER_NONE, DO_NOTHING, invalid, invalid};
	_history.push_back(HistoryEntry{situation, action, 0});
	++_position_repetition[situation];
}
