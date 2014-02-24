/** @file Rules.h
 *
 */

#ifndef Rules_h
#define Rules_h

#include "types.h"
#include "GameModel.h"
class Board;
class Move;
class Piece;
class Square;

/** Provides information about what moves are legal.
 *
 */
class Rules
{
public:
    /** Tries to guess appropriate flags for a move from src to dest.
	 * If there is a legal move that involves moving a piece from src to dest, 
	 * than this function will return that move.  If no legal move exists, the
	 * behaviour is undefined.
	 * When a player clicks some tiles on the gui you can use this function
	 * to guess what the player probably wants and then check whether that
	 * move is legal afterwards with isMoveLegal.
	 */
    Move examineMove(const GameModel &, Square src, Square dest);
	
    /** Checks whether a particular move is legal by standard chess rules.
	 */
    bool isMoveLegal(const GameModel &, Move);
	
	/** 
	 */
	bool isCastlingLegal(const GameModel &, Square src, Square dest);
	 
	/*
	 */
	bool isEnPassantLegal(const GameModel &, Square src, Square dest);
	
	/* checks whether a move is legal that is not a castling or en passant.
	 */
	bool isRegularMoveLegal(const GameModel &, Square src, Square dest, bool capture_flag);
	
	Square getKingStartingSquare(const Board &board, Player player);
	Square getRookStartingSquare(const Board &board, Player player, CastlingType type);
	
	/** Returns true if the players king is attacked
	 */
	bool isPlayerInCheck(const Board &board, Player player);
	
	/** Returns true if the player attacks a square with one of his pieces
	 */
	bool doesPlayerAttackSquare(const Board &board, Square square, Player player);
	
	/** returns true if there are no other pieces between src and dest.
	 * Both squares need to be connected by a straight or diagonal line,
	 * otherwise the behaviour is undefined.
	 */
	bool isPathFree(const Board &board, Square src, Square dest);
	
    /** Check whether a move fullfills the general movement pattern of a piece.
	 * For example, this function would return true if there is a bishop on
	 * src and dest is inside the board and in a diagonal line from src.  Any
	 * other pieces on the board are ignored.
     */
    bool isSquareInRange(const Board &board, Square src, Square dest, bool capture_flag);
	
	/** simplified case for most pieces.
	 * The parameters only contain enough information, if type is not
	 * TYPE_PAWN.  If you call this function for a pawn, the result is
	 * undefined.
	 */
    bool isSquareInRange(Type type, Coord d_rank, Coord d_file);
};

#endif // Rules_h
