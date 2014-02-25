#include "Action.hpp"

bool operator==(const Action &lhs, const Action &rhs) {
	if (lhs.player != rhs.player)
		return false;
	if (lhs.type != rhs.type)
		return false;
	switch (lhs.type) {
		case CAPTURE_PIECE:
		case MOVE_PIECE:
		case CASTLING:
			if (lhs.src != rhs.src)
				return false;
			if (lhs.dst != rhs.dst)
				return false;
			break;
		default:
			return true;
	}
	return true;
}

bool operator!=(const Action &lhs, const Action &rhs) {
	return !operator==(lhs, rhs);
}
