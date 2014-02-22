/** @file Rules.h
 *
 */

#ifndef Rules_h
#define Rules_h

#include "types.h"
class GameModel;
class Move;
class Piece;
class Square;

/** Provides information about what moves are legal.
 *
 */
class Rules
{
public:
    /// Tries to guess appropriate flags for a move from src to dest.
    static Move examineMove(const GameModel* gameModel, Square src, Square dest);
    /// Checks, whether the move could be applied to the model.
    static bool isMoveLegal(const GameModel* gameModel, Move move);
    /** Determines, whether a piece could move/capture to another piece, if it
     * was the only piece on the board.
     */
    static bool isSquareInRange(Piece piece, Square src, Square dest, bool capture_flag = false);
    static bool isSquareInRange(Type type, Coord d_rank, Coord d_file);
};

#endif // Rules_h
