#ifndef PIECE_HPP
#define PIECE_HPP

#include "stdtypes.hpp"

enum Player : int8 {
	PLAYER_NONE = -1,
	PLAYER_WHITE,
	PLAYER_BLACK,
};

enum Type : int8 {
	TYPE_NONE = -1,
	TYPE_KING,
	TYPE_QUEEN,
	TYPE_ROOK,
	TYPE_BISHOP,
	TYPE_KNIGHT,
	TYPE_PAWN,
};

struct Piece {
	Player player;
	Type type;
	
	static const Piece NONE;
};

bool operator == (Piece lhs, Piece rhs);
bool operator != (Piece lhs, Piece rhs);

#endif // PIECE_HPP
