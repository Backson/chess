#ifndef POSITION_HPP
#define POSITION_HPP

#include "Action.hpp"
#include "Board.hpp"

enum GameState {
	STATE_START,
	STATE_WHITE_TURN,
	STATE_BLACK_TURN,
	STATE_WHITE_WIN,
	STATE_BLACK_WIN,
	STATE_DRAW,
};

class Position {
public:
	// LIFECYCLE
	Position();
	Position(const Position& other);
	Position& operator=(const Position& other);

	Position(const Board& board, GameState game_state, int en_passant_chance_file = -1);

	// OPERATORS
	bool operator==(const Position& other) const;
	bool operator!=(const Position& other) const;

	// ACCESS
	const Board& board() const;
	Board& board();
	GameState game_state() const;
	bool is_playing() const;
	Player active_player() const;
	int half_turn_counter() const;

	int8 en_passant_chance_file() const;
	bool can_castle(CastlingType type, Player player) const;

	// OPERATIONS
	void start();
	void action(const Action& action);

private:
	Board _board;
	GameState _game_state;

	int8 _en_passant_chance_file;
	bool _castling_chances[2][2]; // ...[player][castling_type]

    /// how many half turns without a capture or pawn move
	int _half_turn_counter;
};

#endif // POSITION_HPP
