#ifndef SITUATION_HPP
#define SITUATION_HPP

#include "Position.hpp"

class Situation :
	public Position
{
public:
	// LIFECYCLE
	using Position::Position;
	Situation() = default;

	// ACCESS
	int half_move_counter() const;
	int &half_move_counter();

	bool has_remis_offer() const;
	bool &has_remis_offer();

	bool has_game_ended() const;
	bool &has_game_ended();

	Player winner() const;
	Player &winner();

	// OPERATIONS
	void action(const Action &, Delta *delta = nullptr);

private:
	int _half_move_counter = 0;
	bool _has_remis_offer = false;
	bool _has_game_ended = false;
	Player _winner = PLAYER_NONE;
};

#endif // SITUATION_HPP
