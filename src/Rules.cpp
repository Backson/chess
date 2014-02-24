/** @file Rules.cpp
 *
 */

#include "Rules.h"

#include "Board.h"
#include "GameModel.h"
#include "Move.h"
#include "Piece.h"
#include "Square.h"
#include "types.h"

Move Rules::examineMove(const GameModel &model, Square src, Square dest) {
    Move m;

    m.src = src;
    m.dest = dest;

    const Board& board = model.getBoard();

    m.player = board.getPiece(src).getPlayer();

    // check for promotion (assume queen)
	m.promotion = TYPE_NONE;
    if (board.getPiece(src).getType() == TYPE_PAWN) {
        switch (m.player) {
        case PLAYER_WHITE:
            m.promotion = dest.getRank() == 7 ? TYPE_QUEEN : TYPE_NONE;
            break;
        case PLAYER_BLACK:
            m.promotion = dest.getRank() == 0 ? TYPE_QUEEN : TYPE_NONE;
            break;
        default:
            break;
        }
    }

    m.en_passant_flag = false;
    m.capture_flag = board.getPiece(dest).getType() != TYPE_NONE;
    m.castling_flag = false;
	if (board.getPiece(src).getType() == TYPE_KING) {
		Coord file_d = m.dest.getFile() - m.src.getFile();
		if (file_d * file_d == 4)
			m.castling_flag = true;
	}

    return m;
}

bool Rules::isMoveLegal(const GameModel &model, Move move) {
	Player player = move.player;
	
	const Board &board = model.getBoard();
    Piece piece = board.getPiece(move.src);
	
    // is it this players turn?
    switch (model.getGameState()) {
    case STATE_WHITE_TURN:
        if (move.player != PLAYER_WHITE)
            return false;
        break;
    case STATE_BLACK_TURN:
        if (move.player != PLAYER_BLACK)
            return false;
        break;
    default:
        return false;
    }

    // is there a piece on src?
    if (piece.getType() == TYPE_NONE)
        return false; // player wants to move an empty piece

    // is the piece owned by the player making the move?
    if (piece.getPlayer() != player)
        return false; // player wants to move his opponent's pieces

    // has the piece actually moved?
    if (move.src == move.dest)
        return false; // the piece must not stay on its square
	
	// special cases
    if (move.castling_flag) {
		if (move.capture_flag || move.en_passant_flag)
			return false;
		if (!isCastlingLegal(model, move.src, move.dest))
			return false;
    } else if (move.en_passant_flag) {
		if (!move.capture_flag)
			return false;
		if (!isEnPassantLegal(model, move.src, move.dest))
			return false;
	} else {
		if (!isRegularMoveLegal(model, move.src, move.dest, move.capture_flag))
			return false;
	}

	// do we end our turn in check?
    GameModel result = model; // explicitly copy the current position
	result.move(move); // apply the move (should otherwise be legal by now)
	if (isPlayerInCheck(result.getBoard(), player))
		return false;
	
	// passed all tests
	return true;
}

bool Rules::isCastlingLegal(const GameModel &model, Square src, Square dest) {
	const Board &board = model.getBoard();
	Piece piece = board.getPiece(src);
	Player player = piece.getPlayer();
	
	// only allow castling for kings in the starting position
	if (getKingStartingSquare(board, player) != src)
		return false;
	if (piece.getType() != TYPE_KING)
		return false;

	// which of the two castlings is being performed?
	Coord file_d = dest.getFile() - src.getFile();
	CastlingType castling_type;
	if (file_d == +2)
		castling_type = KINGSIDE;
	else if (file_d == -2)
		castling_type = QUEENSIDE;
	else
		return false;
	
	// is there a rook where it needs to be?
	Square rook_square = getRookStartingSquare(board, player, castling_type);
	if (board.getPiece(rook_square).getType() != TYPE_ROOK)
		return false;
	
	// was the king or the rook previously moved?
	if (!model.canCastle(castling_type, player))
		return false;
	
	// is there any piece between the king and the rook?
	if (!isPathFree(board, src, rook_square))
		return false;
	
	// is the player performing the castling currently in check?
	Player opponent = player == PLAYER_WHITE ? PLAYER_BLACK : PLAYER_WHITE;
	if (doesPlayerAttackSquare(board, src, opponent))
		return false;
	
	// is the square the king passes over currently attacked?
	Coord file_step = castling_type == KINGSIDE ? +1 : -1;
	Coord file_passing_by = src.getFile() + file_step;
	Square square_passing_by = Square(src.getRank(), file_passing_by);
	if (doesPlayerAttackSquare(board, square_passing_by, opponent))
		return false;
	
	return true;
}

bool Rules::isEnPassantLegal(const GameModel &model, Square src, Square dest) {
	return false;
}

bool Rules::isRegularMoveLegal(const GameModel &model, Square src, Square dest, bool capture_flag) {
	
	const Board &board = model.getBoard();
	Piece src_piece = board.getPiece(src);
	Piece dest_piece = board.getPiece(dest);
	
	// can the piece actually move there (ignoring other pieces on the board)?
	if (!isSquareInRange(board, src, dest, capture_flag))
		return false;
	
	// capturing moves
	if (capture_flag) {
		if (dest_piece.getType() == TYPE_NONE)
			return false;
		if (src_piece.getPlayer() == dest_piece.getPlayer())
			return false;
	} else {
		// target square must be empty for non-capture moves
		if (dest_piece.getType() != TYPE_NONE)
			return false;
	}
	
	// check whether the path between src and dest is actually free
	if (src_piece.getType() != TYPE_KNIGHT)
		if (!isPathFree(board, src, dest))
			return false;
	
	return true;
}

Square Rules::getKingStartingSquare(const Board &board, Player player) {
	if (player == PLAYER_WHITE)
		return Square(0, 4);
	else if (player == PLAYER_BLACK)
		return Square(board.getHeight() - 1, 4);
	else
		return Square::INVALID;
}

Square Rules::getRookStartingSquare(const Board &board, Player player, CastlingType type) {
	Coord rank, file;
	if (player == PLAYER_WHITE)
		rank = 0;
	else if (player == PLAYER_BLACK)
		rank = board.getHeight() - 1;
	else
		return Square::INVALID;
	if (type == KINGSIDE)
		file = board.getWidth() - 1;
	else if (type == QUEENSIDE)
		file = 0;
	else
		return Square::INVALID;
	return Square(rank, file);
}

bool Rules::isPlayerInCheck(const Board &board, Player player) {
	// find players king
	Square king = Square::INVALID;
	for (Coord rank = 0; rank < board.getHeight(); ++rank)
	for (Coord file = 0; file < board.getWidth(); ++file) {
		Square square = Square(rank, file);
		Piece piece = board.getPiece(square);
		if (piece.getType() == TYPE_KING && piece.getPlayer() == player) {
			king = square;
			goto BREAK_FROM_NESTED_LOOP;
		}
	}
	BREAK_FROM_NESTED_LOOP:;
	
	Player opponent = player == PLAYER_WHITE ? PLAYER_BLACK : PLAYER_WHITE;
	return doesPlayerAttackSquare(board, king, opponent);
}

bool Rules::doesPlayerAttackSquare(const Board &board, Square square, Player player) {
	// en passant capture is completely ignored for this function.
	static const Coord rank_steps[] = {-1, -1, -1,  0,  0, +1, +1, +1};
	static const Coord file_steps[] = {-1,  0, +1, -1, +1, -1,  0, +1};
	
	// for all eight directions
	for (int i = 0; i < 8; ++i) {
		// walk in that direction...
		Coord rank = square.getRank() + rank_steps[i];
		Coord file = square.getFile() + file_steps[i];
		// ...until we hit the edge of the board...
		while (board.isInBound(rank, file)) {
			Square dest = Square(rank, file);
			Piece piece = board.getPiece(dest);
			// ...or we find a piece in our way.
			if (piece.getType() != TYPE_NONE) {
				bool is_opponent = piece.getPlayer() == player;
				bool can_reach_me = isSquareInRange(board, dest, square, true);
				if (is_opponent && can_reach_me) {
					return true;
				} else {
					goto NEXT_DIRECTION;
				}
			}
			rank += rank_steps[i];
			file += file_steps[i];
		}
		NEXT_DIRECTION:;
	} // for each of the eight direction
	
	// check for knights
	static const Coord rank_knight_steps[] = {-1, -1, +1, +1, -2, -2, +2, +2};
	static const Coord file_knight_steps[] = {-2, +2, -2, +2, -1, +1, -1, +1};
	for (int i = 0; i < 8; ++i) {
		Coord rank = square.getRank() + rank_knight_steps[i];
		Coord file = square.getFile() + file_knight_steps[i];
		Square dest = Square(rank, file);
		if (!board.isInBound(dest))
			continue;
		Piece piece = board.getPiece(dest);
		if (piece.getType() != TYPE_NONE) {
			bool is_opponent = piece.getPlayer() == player;
			bool can_reach_me = isSquareInRange(board, dest, square, true);
			if (is_opponent && can_reach_me) {
				return true;
			}
		}
	} // check all eight knight positions
	
	// nothing found
	return false;
}

bool Rules::isPathFree(const Board &board, Square src, Square dest) {
	if (!board.isInBound(src) || !board.isInBound(dest))
		return false;
	if (board.getPiece(src).getType() == TYPE_KNIGHT)
		return true;
	Coord rank_d = dest.getRank() - src.getRank();
	Coord file_d = dest.getFile() - src.getFile();
	Coord rank_step, file_step;
	if (rank_d > 0)
		rank_step = +1;
	else if (rank_d < 0)
		rank_step = -1;
	else
		rank_step = 0;
	if (file_d > 0)
		file_step = +1;
	else if (file_d < 0)
		file_step = -1;
	else
		file_step = 0;
	if (rank_d != 0 && file_d != 0 && rank_d * rank_d != file_d * file_d) {
		// Input is no straight or diagonal line
		return false;
	}
	
	Coord rank = src.getRank() + rank_step;
	Coord file = src.getFile() + file_step;
	
	while (rank != dest.getRank() || file != dest.getFile()) {
		Piece piece = board.getPiece(rank, file);
		if (piece.getType() != TYPE_NONE)
			return false;
		rank += rank_step;
		file += file_step;
	}
	
	return true;
}

bool Rules::isSquareInRange(const Board &board, Square src, Square dest, bool capture_flag) {
	Piece piece = board.getPiece(src);
    Coord d_rank = dest.getRank() - src.getRank();
    Coord d_file = dest.getFile() - src.getFile();

    // sort out moves, which end on the src square or outside the board
    if (d_rank == 0 && d_file == 0)
        return false;
    if (!board.isInBound(src) || !board.isInBound(dest))
        return false;

	/* Pawns are the only pieces with non trivial moving patterns.  They
	 * behave differently for each player (because direction matters) and
	 * also capture moves differ from non-capture ones.  This is why they get
	 * a special treatment here.
	 */
    switch (piece.getType())
    {
    case TYPE_PAWN: {
		// generalize this section for both players
		Coord direction;
		Coord start_rank;
		switch (piece.getPlayer()) {
			case PLAYER_WHITE:
				direction = +1;
				start_rank = 1;
				break;
			case PLAYER_BLACK:
				direction = -1;
				start_rank = board.getHeight() - 2;
				break;
			default: return false;
		}
		if (capture_flag) {
			// all capture moves must be one tile forward...
			if (d_rank != 1 * direction)
				return false;
			// ...and exactly one tile sideway either way.
			if (d_file != 1 && d_file != -1)
				return false;
			return true;
		} else {
			// all non-capture pawn moves must be straight forward
			if (d_file != 0)
				return false;
			// going forward two tiles is only possible from starting position
			if (d_rank == 2 * direction)
				return src.getRank() == start_rank;
			// other than that, pawns can only move forward single tiles
			if (d_rank == 1 * direction)
				return true;
			return false;
		}
	} // case TYPE_PAWN
	/*case TYPE_KING:
		if (castling_flag)
			return d_rank == 0 && d_file * d_file == 4;
		// intentional falltrough*/
    default:
        return isSquareInRange(piece.getType(), d_rank, d_file);
	} // switch (piece.getType())
}

bool Rules::isSquareInRange(Type type, Coord d_rank, Coord d_file) {
    // sort out moves, which end on the src square
    if (d_rank == 0 && d_file == 0)
        return false;

    switch (type) {
    case TYPE_KING:
		return d_rank * d_rank + d_file * d_file <= 2;
    case TYPE_QUEEN:
		return d_rank == 0 || d_file == 0 || d_rank * d_rank == d_file * d_file;
    case TYPE_ROOK:
        return d_rank == 0 || d_file == 0;
    case TYPE_BISHOP:
        return d_rank * d_rank == d_file * d_file;
    case TYPE_KNIGHT:
        return d_rank * d_rank + d_file * d_file == 5;
    case TYPE_PAWN:
        return false; // pawns can't be checked this way
    default:
        return false; // illegal piece type
    }
}
