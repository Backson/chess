#include "Rules.hpp"

#include "Position.hpp"

Action Rules::examineMove(const Position &position, Tile src, Tile dst) {
	Player player = position.active_player();
	Tile invalid = position.INVALID_TILE;

	if (!position.isInBound(src) || !position.isInBound(dst)) {
		return {player, DO_NOTHING, invalid, invalid, TYPE_NONE};
	} else if (position[dst] != Piece::NONE) {
		return {player, CAPTURE_PIECE, src, dst, TYPE_NONE};
	} else if (position[src].type == TYPE_KING && (dst - src).norm2() > 2) {
		return {player, CASTLING, src, dst, TYPE_NONE};
	} else if (position[src].type == TYPE_PAWN && (dst - src).norm2() == 2) {
		return {player, EN_PASSANT, src, dst, TYPE_NONE};
	} else {
		return {player, MOVE_PIECE, src, dst, TYPE_NONE};
	}
}

bool Rules::isActionLegal(const Position &position, Action a) {
	// catch any out of bound indices
	if (!position.isInBound(a.src) || !position.isInBound(a.dst))
		return false;

	// is it this players turn?
	a.player = position.active_player();

	// is there a piece on src?
	if (position[a.src].type == TYPE_NONE)
		return false; // player wants to move an empty piece

	// is the piece owned by the player making the move?
	if (position[a.src].player != a.player)
		return false; // player wants to move his opponent's pieces

	// has the piece actually moved?
	if (a.src == a.dst)
		return false; // the piece must not stay on its square

	// special cases
	if (a.type == CASTLING) {
		if (!isCastlingLegal(position, a))
			return false;
	} else if (a.type == EN_PASSANT) {
		if (!isEnPassantLegal(position, a))
			return false;
	} else {
		if (!isRegularMoveLegal(position, a))
			return false;
	}

	// do we end our turn in check?
	Position result = position; // explicitly copy the current position
	result.action(a); // apply the move (should otherwise be legal by now)
	if (isPlayerInCheck(result, a.player))
		return false;

	// passed all tests
	return true;
}

bool Rules::isCastlingLegal(const Position &position, Action a) {
	// only allow castling for kings in the starting position
	if (getKingStartingSquare(position, a.player) != a.src)
		return false;
	if (position[a.src].type != TYPE_KING)
		return false;

	// which of the two castlings is being performed?
	Tile d = a.dst - a.src;
	CastlingType castling_type;
	if (d[0] == +2 && d[1] == 0)
		castling_type = KINGSIDE;
	else if (d[0] == -2 && d[1] == 0)
		castling_type = QUEENSIDE;
	else
		return false;

	// is there a rook where it needs to be?
	Tile rook = getRookStartingSquare(position, a.player, castling_type);
	if (position[rook].type != TYPE_ROOK)
		return false;

	// was the king or the rook previously moved?
	if (!position.can_castle(a.player, castling_type))
		return false;

	// is there any piece between the king and the rook?
	if (!isPathFree(position, a.src, rook))
		return false;

	// is the player performing the castling currently in check?
	Player opponent = a.player == PLAYER_WHITE ? PLAYER_BLACK : PLAYER_WHITE;
	if (doesPlayerAttackSquare(position, a.src, opponent))
		return false;

	// is the square the king passes over currently attacked?
	Tile step = Tile((int8)(castling_type == KINGSIDE ? +1 : -1), (int8)0);
	Tile passing_by = a.src + step;
	if (doesPlayerAttackSquare(position, passing_by, opponent))
		return false;

	return true;
}

bool Rules::isEnPassantLegal(const Position &position, Action a) {
	// only pawns can capture en passant
	if (position[a.src].type != TYPE_PAWN)
		return false;

	// the pawn must be able to reach dst by a capture move
	if (!isSquareInRange(position, a.src, a.dst, true))
		return false;

	// but that tile needs to be empty
	if (position[a.dst] != Piece::NONE)
		return false;

	// there needs to be an enemy pawn that has passed the current pawn
	Tile opponent_pawn = Tile(a.dst[0], a.src[1]);
	Player opponent = a.player == PLAYER_WHITE ? PLAYER_BLACK : PLAYER_WHITE;
	if (position[opponent_pawn] != Piece{opponent, TYPE_PAWN})
		return false;

	// there needs to be an en passant chance in that particular column
	if (position.en_passant_file() != a.dst[0])
		return false;

	return true;
}

bool Rules::isRegularMoveLegal(const Position &position, Action a) {
	// can the piece actually move there (ignoring other pieces on the board)?
	if (!isSquareInRange(position, a.src, a.dst, a.type == CAPTURE_PIECE))
		return false;

	// capturing moves
	if (a.type == CAPTURE_PIECE) {
		// can't capture empty tiles
		if (position[a.dst].type == TYPE_NONE)
			return false;
		// can't capture your own pieces
		if (position[a.dst].player == a.player)
			return false;
	} else {
		// target square must be empty for non-capture moves
		if (position[a.dst].type != TYPE_NONE)
			return false;
	}

	// check for promotions
	Coord end_row = a.player == PLAYER_WHITE ? position.width() - 1 : 0;
	if (position[a.src].type == TYPE_PAWN && a.dst[1] == end_row) {
		switch (a.promotion) {
			case TYPE_NONE:
			case TYPE_PAWN:
			case TYPE_KING:
				return false;
			case TYPE_QUEEN:
			case TYPE_ROOK:
			case TYPE_BISHOP:
			case TYPE_KNIGHT:
				break;
		}
	}

	// check whether the path between src and dest is actually free
	if (position[a.src].type != TYPE_KNIGHT)
		if (!isPathFree(position, a.src, a.dst))
			return false;

	return true;
}

Tile Rules::getKingStartingSquare(const Board &board, Player player) {
	if (player == PLAYER_WHITE)
		return Tile((int8)4, (int8)0);
	else if (player == PLAYER_BLACK)
		return Tile((int8)4, (int8)(board.height() - 1));
	else
		return Board::INVALID_TILE;
}

Tile Rules::getRookStartingSquare(const Board &board, Player player, CastlingType type) {
	int8 x, y;
	if (player == PLAYER_WHITE)
		y = 0;
	else if (player == PLAYER_BLACK)
		y = board.height() - 1;
	else
		return Board::INVALID_TILE;
	if (type == KINGSIDE)
		x = board.width() - 1;
	else if (type == QUEENSIDE)
		x = 0;
	else
		return Board::INVALID_TILE;
	return Tile(x, y);
}

bool Rules::isPlayerInCheck(const Board &board, Player player) {
	// find players king
	Tile king = Board::INVALID_TILE;
	for (int8 y = 0; y < board.height(); ++y)
		for (int8 x = 0; x < board.width(); ++x) {
			Tile tile = Tile(x, y);
			if (board[tile].type == TYPE_KING && board[tile].player == player) {
				king = tile;
				goto BREAK_FROM_NESTED_LOOP;
			}
		}
	BREAK_FROM_NESTED_LOOP:;

	Player opponent = player == PLAYER_WHITE ? PLAYER_BLACK : PLAYER_WHITE;
	return doesPlayerAttackSquare(board, king, opponent);
}

bool Rules::doesPlayerAttackSquare(const Board &board, Tile tile, Player p) {
	// en passant capture is completely ignored for this function.
	static const int8 sx[] = {-1, -1, -1,  0,  0, +1, +1, +1};
	static const int8 sy[] = {-1,  0, +1, -1, +1, -1,  0, +1};

	// for all eight directions
	for (int i = 0; i < 8; ++i) {
		// walk in that direction...
		Tile s = Tile(sx[i], sy[i]);
		Tile iter = tile + s;
		// ...until we hit the edge of the board...
		while (board.isInBound(iter)) {
			// ...or we find a piece in our way.
			if (board[iter].type != TYPE_NONE) {
				bool is_opponent = board[iter].player == p;
				bool can_reach_me = isSquareInRange(board, iter, tile, true);
				if (is_opponent && can_reach_me) {
					return true;
				} else {
					goto NEXT_DIRECTION;
				}
			}
			iter += s;
		}
		NEXT_DIRECTION:;
	} // for each of the eight direction

	// check for knights
	static const int8 sx_knight[] = {-1, -1, +1, +1, -2, -2, +2, +2};
	static const int8 sy_knight[] = {-2, +2, -2, +2, -1, +1, -1, +1};
	for (int i = 0; i < 8; ++i) {
		Tile knight = tile + Tile(sx_knight[i], sy_knight[i]);
		if (!board.isInBound(knight))
			continue;
		if (board[knight].type != TYPE_NONE) {
			bool is_opponent = board[knight].player == p;
			bool can_reach_me = isSquareInRange(board, knight, tile, true);
			if (is_opponent && can_reach_me) {
				return true;
			}
		}
	} // check all eight knight positions

	// nothing found
	return false;
}

bool Rules::isPathFree(const Board &board, Tile src, Tile dst) {
	if (!board.isInBound(src) || !board.isInBound(dst))
		return false;
	// knights can go anywhere, no matter if pieces are in the way
	if (board[src].type == TYPE_KNIGHT)
		return true;
	Tile d = dst - src;

	// check for non-straight or -diagonal lines
	if (d[0] != 0 && d[1] != 0 && d[0] * d[0] != d[1] * d[1])
		return false;
	Tile step;
	for (int i = 0; i < 2; ++i)
		if (d[i] > 0)
			step[i] = +1;
		else if (d[i] < 0)
			step[i] = -1;
		else
			step[i] = 0;

	// look for pieces on the path
	Tile iter = src + step;
	while (iter != dst) {
		if (board[iter].type != TYPE_NONE)
			return false;
		iter += step;
	}

	return true;
}

bool Rules::isSquareInRange(const Board &board, Tile src, Tile dst, bool capture) {
	Tile d = dst - src;

	// sort out moves, which end on the src square or outside the board
	if (!board.isInBound(src) || !board.isInBound(dst))
		return false;
	if (d.norm2() == 0)
		return false;

	/* Pawns are the only pieces with non trivial moving patterns.  They
	 * behave differently for each player (because direction matters) and
	 * also capture moves differ from non-capture ones.  This is why they get
	 * a special treatment here.
	 */
	switch (board[src].type)
	{
	case TYPE_PAWN: {
		// generalize this section for both players
		int8 direction;
		int8 start_row;
		switch (board[src].player) {
			case PLAYER_WHITE:
				direction = +1;
				start_row = 1;
				break;
			case PLAYER_BLACK:
				direction = -1;
				start_row = board.height() - 2;
				break;
			default: return false;
		}
		if (capture) {
			// all capture moves must be one tile forward...
			if (d[1] != 1 * direction)
				return false;
			// ...and exactly one tile sideway either way.
			if (d[0] != 1 && d[0] != -1)
				return false;
			return true;
		} else {
			// all non-capture pawn moves must be straight forward
			if (d[0] != 0)
				return false;
			// going forward two tiles is only possible from starting position
			if (d[1] == 2 * direction)
				return src[1] == start_row;
			// other than that, pawns can only move forward single tiles
			if (d[1] == 1 * direction)
				return true;
			return false;
		}
	} // case TYPE_PAWN
	default:
		return isSquareInRange(board[src].type, d);
	} // switch (piece.getType())
}

bool Rules::isSquareInRange(Type type, Tile d) {
	// sort out moves, which end on the src square
	if (d.norm2() == 0)
		return false;

	switch (type) {
	case TYPE_KING:
		return d.norm2() <= 2;
	case TYPE_QUEEN:
		return d[0] == 0 || d[1] == 0 || d[0] * d[0] == d[1] * d[1];
	case TYPE_ROOK:
		return d[0] == 0 || d[1] == 0;
	case TYPE_BISHOP:
		return d[0] * d[0] == d[1] * d[1];
	case TYPE_KNIGHT:
		return d.norm2() == 5;
	case TYPE_PAWN:
		return false; // pawns can't be checked this way
	default:
		return false; // illegal piece type
	}
}

bool Rules::hasLegalMove(const Position &position, Tile src) {
	for (Coord y = 0; y < position.height(); ++y) {
		for (Coord x = 0; x < position.width(); ++x) {
			if (hasLegalMove(position, src, Tile(x, y)))
				return true;
		}
	}
	return false;
}

bool Rules::hasLegalMove(const Position &position, Tile src, Tile dst) {
	Action action = examineMove(position, src, dst);
	return isActionLegal(position, action);
}
