#include "Action.hpp"

bool operator==(const Action &lhs, const Action &rhs) {
	if (lhs.player != rhs.player)
		return false;
	if (lhs.type != rhs.type)
		return false;
	if (lhs.src != rhs.src)
		return false;
	if (lhs.dst != rhs.dst)
		return false;
	if (lhs.promotion != rhs.promotion)
		return false;
	
	return true;
}

bool operator!=(const Action &lhs, const Action &rhs) {
	return !operator==(lhs, rhs);
}
