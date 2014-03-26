#ifndef ACTION_HPP
#define ACTION_HPP

#include "Piece.hpp"
#include "Board.hpp"

/** Aliases for the two types of castling
 */
enum CastlingType : int8 {
	KINGSIDE,
	QUEENSIDE,
};

/** All possible actions that a player can take during a game of chess.
 */
enum ActionType : int8 {
	DO_NOTHING,

	MOVE_PIECE,
	CAPTURE_PIECE,
	CASTLING,
	EN_PASSANT,

	RESIGN,
	ACCEPT_REMIS,
};

enum AnnouncementType : int8 {
	OFFER_REMIS,
	CLAIM_POSITION_REPETITION,
	CLAIM_FIFTY_MOVES_RULE,
};

/** Holds all the information about one legal action that a player can take.
 */
struct Action {
	/** Which player is making the move? */
	Player player;

	/** What does the player want to do? */
	ActionType type;

	/** Which tile is the piece on that the player wants to move */
	Tile src;

	/** Which tile does the player want to move the pawn to? */
	Tile dst;

	/** if the action is a pawn promotion, to what type will it be promoted? */
	Type promotion;

	/** to offer or claim remis */
	AnnouncementType announcement;
};

bool operator==(const Action &, const Action &);
bool operator!=(const Action &, const Action &);

#endif // ACTION_HPP
