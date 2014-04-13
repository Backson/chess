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

void Position::action(const Action &a) {
	const Player opponent = static_cast<Player>(1 - _active_player);

	if(!_hashed)
		hash();

	// check for king moves
	if (piece(a.src).type == TYPE_KING) {
		_can_castle[a.player][KINGSIDE] = false;
		_can_castle[a.player][QUEENSIDE] = false;
		_hash_value ^= zobrist_castling(a.player, KINGSIDE);
		_hash_value ^= zobrist_castling(a.player, QUEENSIDE);
	}

	// set the en passant chance
	_en_passant_file = -1;
	if (piece(a.src).type == TYPE_PAWN && (a.dst - a.src).norm2() == 4) {
		Tile left = a.dst + Tile(-1, 0);
		Tile right = a.dst + Tile(1, 0);
		Piece p = Piece{opponent, TYPE_PAWN};
		bool pawnLeft = isInBound(left) && piece(left) == p;
		bool pawnRight = isInBound(right) && piece(right) == p;
		if(pawnLeft || pawnRight) {
			_en_passant_file = a.dst[0];
			_hash_value ^= zobrist_file(a.dst[0]);
		}
	}

	// check for rook moves
	for (int player = 0; player < 2; ++player)
		for (int castling = 0; castling < 2; ++castling) {
			Coord x = castling == KINGSIDE ? width() - 1 : 0;
			Coord y = player == PLAYER_WHITE ? 0 : height() - 1;
			if (a.src == Tile(x, y) || a.dst == Tile(x, y))
			{
				_can_castle[player][castling] = false;
				_hash_value ^= zobrist_castling((Player) player, (CastlingType) castling);
			}
		}

	// actually move the piece
	movePiece(a.src, a.dst);

	// move the rook (castlings only)
	if (a.type == CASTLING) {
		Coord home_row = a.player == PLAYER_WHITE ? 0 : height() - 1;
		Coord sx = (a.dst - a.src)[0] > 0 ? +1 : -1;
		Tile rook_src = Tile(sx > 0 ? width() - 1 : 0, home_row);
		Tile rook_dst = a.src + Tile(sx, 0);
		movePiece(rook_src, rook_dst);
	}

	// do piece transformations
	if (a.promotion != TYPE_NONE) {
		_hash_value ^= zobrist_piece_tile(piece(a.dst), a.dst, width());
		piece(a.dst) = Piece{piece(a.dst).player, a.promotion};
		_hash_value ^= zobrist_piece_tile(piece(a.dst), a.dst, width());
	}

	// remove pieces that were captured en passant
	if (a.type == EN_PASSANT) {
		removePiece(Tile(a.dst[0], a.src[1]));
	}

	// next player
	_active_player = opponent;
	_hash_value ^= zobrist_player();
}
