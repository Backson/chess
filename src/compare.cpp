#include "compare.hpp"

bool BoardCompare::operator () (const Board &lhs, const Board &rhs) const {
	if (lhs.width() < rhs.width())
		return true;
	if (lhs.width() > rhs.width())
		return false;
	if (lhs.height() < rhs.height())
		return true;
	if (lhs.height() > rhs.height())
		return false;
	auto w = lhs.width();
	auto h = lhs.height();
	for (int8 y = 0; y < h; ++y)
		for (int8 x = 0; x < w; ++x) {
			auto tile = Tile(x, y);
			auto a = lhs[tile];
			auto b = rhs[tile];
			if (a.player < b.player)
				return true;
			if (a.player > b.player)
				return false;
			if (a.type < b.type)
				return true;
			if (a.type > b.type)
				return false;
		}

	return false;
}

bool PositionCompare::operator () (const Position &lhs, const Position &rhs) const {
	BoardCompare board_comp;
	if (board_comp(lhs.board(), rhs.board()))
		return true;
	if (board_comp(rhs.board(), lhs.board()))
		return false;
	for (int player_i = 0; player_i < 2; ++player_i)
		for (int castling_i = 0; castling_i < 2; ++castling_i) {
			auto castling = (CastlingType)castling_i;
			auto player = (Player)player_i;
			bool lhs_castle = lhs.can_castle(castling, player);
			bool rhs_castle = rhs.can_castle(castling, player);
			if (!lhs_castle && rhs_castle)
				return true;
			if (lhs_castle && !rhs_castle)
				return false;
		}

	if (lhs.en_passant_chance_file() < rhs.en_passant_chance_file())
		return true;
	if (lhs.en_passant_chance_file() > rhs.en_passant_chance_file())
		return false;

	// ignore half_turn_counter
//	if (lhs.half_turn_counter() < rhs.half_turn_counter())
//		return true;
//	if (lhs.half_turn_counter() > rhs.half_turn_counter())
//		return false;

	return false;
}
