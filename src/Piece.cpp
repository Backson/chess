#include "Piece.hpp"

bool operator == (Piece lhs, Piece rhs) {
	if (lhs.player != rhs.player)
		return false;
	if (lhs.type != rhs.type)
		return false;
	return true;
}

bool operator != (Piece lhs, Piece rhs) {
	return !operator == (lhs, rhs);
}

Piece &operator ^= (Piece &lhs, Piece rhs) {
	lhs.player = static_cast<Player>(lhs.player ^ rhs.player);
	lhs.type = static_cast<Type>(lhs.type ^ rhs.type);
	return lhs;
}

Piece operator ^ (Piece lhs, Piece rhs) {
	Piece piece = lhs;
	piece ^= rhs;
	return piece;
}

const Piece Piece::NONE = Piece{PLAYER_NONE, TYPE_NONE};
