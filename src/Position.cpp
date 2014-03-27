#include "Position.hpp"

#include "Board.hpp"
#include "Action.hpp"
#include "Piece.hpp"
#include "Rules.hpp"

// LIFECYCLE

Position::Position(const Board &board, Player active_player) :
	Board(board),
	_active_player(active_player)
{
    // nothing
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

    // check for king moves
	if (piece(a.src).type == TYPE_KING) {
		_can_castle[a.player][KINGSIDE] = false;
		_can_castle[a.player][QUEENSIDE] = false;
	}

    // set the en passant chance
    _en_passant_file = -1;
	if (piece(a.src).type == TYPE_PAWN && (a.dst - a.src).norm2() == 4) {
		Tile left = a.dst + Tile(-1, 0);
		Tile right = a.dst + Tile(1, 0);
		Piece p = Piece{opponent, TYPE_PAWN};
		bool pawnLeft = isInBound(left) && piece(left) == p;
		bool pawnRight = isInBound(right) && piece(left) == p;
		if(pawnLeft || pawnRight) {
			_en_passant_file = a.dst[0];
		}
	}

    // check for rook moves
    for (int player = 0; player < 2; ++player)
        for (int castling = 0; castling < 2; ++castling) {
            Coord x = castling == KINGSIDE ? width() - 1 : 0;
            Coord y = player == PLAYER_WHITE ? 0 : height() - 1;
            if (a.src == Tile(x, y) || a.dst == Tile(x, y))
                _can_castle[player][castling] = false;
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
		piece(a.dst) = Piece{piece(a.dst).player, a.promotion};
	}

    // remove pieces that were captured en passant
	if (a.type == EN_PASSANT) {
		removePiece(Tile(a.dst[0], a.src[1]));
	}

    // next player
    _active_player = opponent;
}
