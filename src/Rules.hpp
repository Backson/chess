#ifndef RULES_HPP
#define RULES_HPP

#include "Game.hpp"

#include <vector>

/** Provides information about what moves are legal.
 *
 */
class Rules
{
public:
	/** Tries to guess appropriate flags for a move from src to dest.
	 * If there is a legal move that involves moving a piece from src to dest,
	 * than this function will return that move.  If no legal move exists, the
	 * behavior is undefined.
	 * When a player clicks some tiles on the gui you can use this function
	 * to guess what the player probably wants and then check whether that
	 * move is legal afterwards with isMoveLegal.
	 */
	Action examineMove(const Position &, Tile src, Tile dst);

	/** Checks whether a particular move is legal by standard chess rules.
	 */
	bool isActionLegal(const Position &, Action a);

	/**
	 */
	bool isCastlingLegal(const Position &, Action a);

	/**
	 */
	bool isEnPassantLegal(const Position &, Action a);

	/** checks whether a move is legal that is not a castling or en passant.
	 */
	bool isRegularMoveLegal(const Position &, Action a);

	Tile getKingStartingSquare(const Board &board, Player player);
	Tile getRookStartingSquare(const Board &board, Player player, CastlingType type);

	/** Returns true if the players king is attacked
	 */
	bool isPlayerInCheck(const Board &board, Player player);

	/** Returns true if the player attacks a square with one of his pieces
	 */
	bool doesPlayerAttackSquare(const Board &board, Tile tile, Player p);

	/** returns true if there are no other pieces between src and dest.
	 * Both squares need to be connected by a straight or diagonal line,
	 * otherwise the behaviour is undefined.
	 */
	bool isPathFree(const Board &board, Tile src, Tile dst);

	/** Check whether a move fulfills the general movement pattern of a piece.
	 * For example, this function would return true if there is a bishop on
	 * src and dest is inside the board and in a diagonal line from src.  Any
	 * other pieces on the board are ignored.
	 */
	bool isSquareInRange(const Board &board, Tile src, Tile dst, bool capture_flag);

	/** simplified case for most pieces.
	 * The parameters only contain enough information, if type is not
	 * TYPE_PAWN.  If you call this function for a pawn, the result is
	 * undefined.
	 */
	bool isSquareInRange(Type type, Tile d);

	/** Checks whether a piece on a specific tile has any legal moves to make
	 */
	bool hasLegalMove(const Position &, Tile src);

	/** Checks whether a legal move from src to dst exists
	 */
	bool hasLegalMove(const Position &, Tile src, Tile dst);

    static const int EVERY_PROMOTION = 0x01;
    static const int DRAW_CLAIMS     = 0x02;

	/** get a list with all legal moves
	 */
    std::vector<Action> &getAllLegalMoves(const Game &, std::vector<Action> &, int flags = 0);
    std::vector<Action> getAllLegalMoves(const Game &, int flags = 0);
    std::vector<Action> &getAllLegalMoves(const Situation &, std::vector<Action> &, int flags = 0);
    std::vector<Action> getAllLegalMoves(const Situation &, int flags = 0);
    std::vector<Action> &getAllLegalMoves(const Position &, std::vector<Action> &, int flags = 0);
    std::vector<Action> getAllLegalMoves(const Position &, int flags = 0);
};

#endif // RULES_HPP
