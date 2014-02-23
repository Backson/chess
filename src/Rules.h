/** @file Rules.h
 *
 */

#ifndef Rules_h
#define Rules_h

#include "types.h"
class GameModel;
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
    Move examineMove(const GameModel &gameModel, Square src, Square dest);
	
    /** Checks whether a particular move is legal by standard chess rules.
	 */
    bool isMoveLegal(const GameModel &gameModel, Move move);
	
	bool isPlayerInCheck(const Board &board, Player player);
	bool doesPlayerAttackSquare(const Board &board, Square square, Player player);
	bool isPathFree(const Board &board, Square src, Square dest);
	
    /** Check whether a move fullfills the general movement pattern of a piece.
	 * For example, this function would return true if there is a bishop on
	 * src and dest is inside the board and in a diagonal line from src.  Any
	 * other pieces on the board are ignored.
     */
    bool isSquareInRange(const Board &board, Square src, Square dest, bool capture_flag = false);
	
	/** simplified case for most pieces.
	 * The parameters only contain enough information, if type is not
	 * TYPE_PAWN.  If you call this function for a pawn, the result is
	 * undefined.
	 */
    bool isSquareInRange(Type type, Coord d_rank, Coord d_file);
};

#endif // Rules_h
