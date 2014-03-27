#ifndef POSITION_HPP
#define POSITION_HPP

#include "Action.hpp"
#include "Board.hpp"

class Position :
    public Board
{
public:
	// LIFECYCLE
	Position();
	Position(Coord width, Coord height);
	Position(const Board &, Player);

	// OPERATORS
	bool operator == (const Position &) const;
	bool operator != (const Position &) const;

	// ACCESS
	Player active_player() const;
	Player &active_player();
	Coord en_passant_file() const;
	Coord &en_passant_file();
	bool can_castle(Player, CastlingType) const;
	bool &can_castle(Player, CastlingType);

	// OPERATIONS
	void action(const Action &action);

private:
	Player _active_player;
	Coord _en_passant_file;
	bool _can_castle[2][2]; // ...[player][castling_type]
};

#endif // POSITION_HPP
