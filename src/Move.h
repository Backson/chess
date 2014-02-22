/** @file Move.h
 *
 */

#ifndef Move_h
#define Move_h

#include "Square.h"
#include "types.h"

/**
 * Holds all information about a move of a piece to another square. Only makes
 * sense when coupled with a GameModel. The move can be applied to a model and
 * either change the model (executing the move) or not change it (rejecting
 * the move) in case it is invalid in the context of the model.
 *
 * @see Rules
 */
class Move
{
public:
    /** The square, with the piece being moved. */
    Square src;

    /** The square, where the piece should go. */
    Square dest;

    /** The player ordering the turn. */
    Player player;

    /** true, if another piece is captured during this move, false otherwise.
     * A legal capturing must at least meet all of the following criteria:
     *
     * @verbatim
     * src              must be         a friendly piece.
     * dest             must be         an opponent piece (if not en passen).
     *                                  empty square (if en passen).
     * capture flag     must be         true.
     * castling flag    must be         false.
     * en_passant flag  must be         false (if not en passant).
     *                                  true (if en passant).
     * promotion        must be         TYPE_NONE (if no promotion).
     *                  must not be     TYPE_NONE (if promotion).
     * @endverbatim
     */
    bool capture_flag;

    /** true, if the turn is a castling, false otherwise.
     * A legal castling must at least meet all of the following criteria:
     *
     * @verbatim
     * src              must be         a king.
     * dest             must be         the target square of the king
     *                                  or a friendly rook.
     * capture flag     must be         false.
     * castling flag    must be         true.
     * en_passant flag  must be         false.
     * promotion        must be         TYPE_NONE.
     * @endverbatim
     */
    bool castling_flag;

    /** true, if an opponent pawn should be captured en passant, false otherwise.
     * A legal en passant must meet all of the following criteria:
     *
     * @verbatim
     * src              must be         a friendly piece
     * dest             must be         an empty square.
     * capture flag     must be         true.
     * castling flag    must be         false.
     * en_passant flag  must be         true.
     * promotion        must be         TYPE_NONE.
     * @endverbatim
     */
    bool en_passant_flag;

    /** holds the type, a pawn should be promoted to or TYPE_NONE for moves without promotion.
     * A legal promotion must meet all of the following criteria:
     *
     * @verbatim
     * src              must be         a friendly pawn.
     * dest             must be         8th rank for white and 1st for black.
     * capture flag     must be         true (if dest was holding a piece).
     *                                  false (if dest was empty).
     * castling flag    must be         false.
     * en_passant flag  must be         false.
     * promotion        must not be     TYPE_NONE.
     * @endverbatim
     */
    Type promotion;
};

#endif // Move_h
