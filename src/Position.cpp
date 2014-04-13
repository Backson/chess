#include "Position.hpp"

#include "Board.hpp"
#include "Action.hpp"
#include "Piece.hpp"
#include "Rules.hpp"
#include "zobrist.hpp"

// LIFECYCLE

Position::Position(const Board &board, Player p, CastlingInit castling_init) :
	Board(board),
	_active_player(p)
{
	switch (castling_init) {
	case CASTLING_ALL_FALSE:
		break;
	case CASTLING_ALL_TRUE:
		_can_castle[0][0] = true;
		_can_castle[0][1] = true;
		_can_castle[1][0] = true;
		_can_castle[1][1] = true;
		break;
	case CASTLING_GUESS: {
		Rules rules;
		for (int p = 0; p < 2; ++p)
		for (int c = 0; c < 2; ++c) {
			CastlingType castling = static_cast<CastlingType>(c);
			Player player = static_cast<Player>(p);
			Tile king = rules.getKingStartingSquare(board, player);
			Tile rook = rules.getRookStartingSquare(board, player, castling);
			bool king_ok = board[king] == Piece{player, TYPE_KING};
			bool rook_ok = board[rook] == Piece{player, TYPE_ROOK};
			_can_castle[player][castling] = king_ok && rook_ok;
		}
	}
	} // switch castling_init
}

// OPERATORS

bool Position::operator == (const Position &other) const {
	if (Board::operator != (other))
		return false;

	if (_active_player != other._active_player)
		return false;
	if (_en_passant_file != other._en_passant_file)
		return false;
	if (_can_castle[0][0] != other._can_castle[0][0])
		return false;
	if (_can_castle[0][1] != other._can_castle[0][1])
		return false;
	if (_can_castle[1][0] != other._can_castle[1][0])
		return false;
	if (_can_castle[1][1] != other._can_castle[1][1])
		return false;

	return true;
}

bool Position::operator != (const Position &other) const {
	return !operator == (other);
}

// ACCESS

Player Position::active_player() const {
	return _active_player;
}

Player &Position::active_player() {
	return _active_player;
}

Coord Position::en_passant_file() const {
	return _en_passant_file;
}

Coord &Position::en_passant_file() {
	return _en_passant_file;
}

bool Position::can_castle(Player player, CastlingType type) const {
	return _can_castle[player][type];
}

bool &Position::can_castle(Player player, CastlingType type) {
	return _can_castle[player][type];
}

// OPERATIONS

void Position::action(const Action &a, Delta *delta) {
	const Player opponent = static_cast<Player>(1 - _active_player);

	if (delta) {
		delta->castling_xor = 0;
	}

	// check for king moves
	if (piece(a.src).type == TYPE_KING) {
		if (delta) {
			int shift = a.player == PLAYER_WHITE ? 0 : 2;
			bool kingside = _can_castle[a.player][KINGSIDE];
			bool queenside = _can_castle[a.player][QUEENSIDE];
			delta->castling_xor |= ((kingside ? 1 : 0) << (shift + KINGSIDE));
			delta->castling_xor |= ((queenside ? 1 : 0) << (shift + QUEENSIDE));
		}
		_can_castle[a.player][KINGSIDE] = false;
		_can_castle[a.player][QUEENSIDE] = false;
	}

	// set the en passant chance
	Coord old_en_passant_file = _en_passant_file;
	_en_passant_file = -1;
	if (piece(a.src).type == TYPE_PAWN && (a.dst - a.src).norm2() == 4) {
		Tile left = a.dst + Tile(-1, 0);
		Tile right = a.dst + Tile(1, 0);
		Piece p = Piece{opponent, TYPE_PAWN};
		bool pawnLeft = isInBound(left) && piece(left) == p;
		bool pawnRight = isInBound(right) && piece(right) == p;
		if(pawnLeft || pawnRight) {
			_en_passant_file = a.dst[0];
		}
	}
	if (delta) {
		delta->en_passant_xor = old_en_passant_file ^ _en_passant_file;
	}

	// check for rook moves
	for (int player = 0; player < 2; ++player)
	for (int castling = 0; castling < 2; ++castling) {
		Coord x = castling == KINGSIDE ? width() - 1 : 0;
		Coord y = player == PLAYER_WHITE ? 0 : height() - 1;
		if (a.src == Tile(x, y) || a.dst == Tile(x, y)) {
			if (delta) {
				int shift = player == PLAYER_WHITE ? 0 : 2;
				bool before = _can_castle[player][castling];
				delta->castling_xor |= ((before ? 1 : 0) << (shift + castling));
			}
			_can_castle[player][castling] = false;
		}
	}

	if (delta) {
		delta->tiles[0] = TileDelta{a.src, piece(a.src) ^ Piece::NONE};
	}

	// actually move the piece
	if (a.promotion != TYPE_NONE) {
		// promotions
		removePiece(a.src);
		Piece new_piece = Piece{a.player, a.promotion};
		if (delta)
			delta->tiles[1] = TileDelta{a.dst, piece(a.dst) ^ new_piece};
		piece(a.dst) = new_piece;
	} else {
		// all other moves
		if (delta) {
			delta->tiles[1] = TileDelta{a.dst, piece(a.src) ^ piece(a.dst)};
		}
		movePiece(a.src, a.dst);
	}

	// move the rook (castlings only)
	if (a.type == CASTLING) {
		Coord home_row = a.player == PLAYER_WHITE ? 0 : height() - 1;
		Coord sx = (a.dst - a.src)[0] > 0 ? +1 : -1;
		Tile rook_src = Tile(sx > 0 ? width() - 1 : 0, home_row);
		Tile rook_dst = a.src + Tile(sx, 0);
		movePiece(rook_src, rook_dst);
		if (delta) {
			delta->tiles[2] = TileDelta{rook_src, piece(rook_dst) ^ Piece::NONE};
			delta->tiles[3] = TileDelta{rook_dst, piece(rook_dst) ^ Piece::NONE};
		}
	}

	// remove pieces that were captured en passant
	else if (a.type == EN_PASSANT) {
		Tile en_passant_tile = Tile(a.dst[0], a.src[1]);
		if (delta) {
			delta->tiles[2] = TileDelta{en_passant_tile, piece(en_passant_tile) ^ Piece::NONE};
		}
		removePiece(en_passant_tile);
		if (delta)
			delta->tiles[3] = TileDelta{Board::INVALID_TILE, Piece::NONE ^ Piece::NONE};
	}

	else if (delta){
		delta->tiles[2] = TileDelta{Board::INVALID_TILE, Piece::NONE ^ Piece::NONE};
		delta->tiles[3] = TileDelta{Board::INVALID_TILE, Piece::NONE ^ Piece::NONE};
	}

	// next player
	_active_player = opponent;
}

void Position::apply(Delta delta) {
	int i = 0;
	Tile tile;
	while (isInBound(tile = delta.tiles[i].tile)) {
        piece(tile) ^= delta.tiles[i++].piece_xor;
	}

	bool *can_castle = &_can_castle[0][0];
	for (int i = 0; i < 4; ++i) {
		can_castle[i] = (delta.castling_xor & (1 << i)) ? !can_castle[i] : can_castle[i];
	}

	_en_passant_file ^= delta.en_passant_xor;

	_active_player = static_cast<Player>(1 - _active_player);
}
