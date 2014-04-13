#ifndef POSITION_HPP
#define POSITION_HPP

#include "Action.hpp"
#include "Board.hpp"

struct TileDelta {
	Tile tile;
	Piece piece_xor;
};

struct Delta {
	TileDelta tiles[4];
	int castling_xor;
	Coord en_passant_xor;
};

class Position :
	public Board
{
public:
	enum CastlingInit {
		CASTLING_GUESS,
		CASTLING_ALL_FALSE,
		CASTLING_ALL_TRUE,
	};

	// LIFECYCLE
	using Board::Board;
	Position() = default;
	Position(const Board &, Player, CastlingInit ci = CASTLING_GUESS);

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
	void action(const Action &action, Delta *delta = nullptr);

	void apply(Delta delta);

private:
	Player _active_player = PLAYER_NONE;
	Coord _en_passant_file = -1;
	// _can_castle[player][castling_type]
	bool _can_castle[2][2] = {{false, false}, {false, false}};
};

#endif // POSITION_HPP
