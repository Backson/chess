#include "Piece.hpp"

bool operator == (Piece lhs, Piece rhs) {
	if (lhs.player != rhs.player)
		return false;
	if (lhs.type != rhs.type)
		return false;
	return true;
}

bool operator != (Piece lhs, Piece rhs) {
	return !operator==(lhs, rhs);
}

const Piece Piece::NONE = Piece{PLAYER_NONE, TYPE_NONE};
