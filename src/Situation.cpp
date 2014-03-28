#include "Situation.hpp"

// ACCESS

int Situation::half_move_counter() const {
	return _half_move_counter;
}

int &Situation::half_move_counter() {
	return _half_move_counter;
}

bool Situation::has_remis_offer() const {
	return _has_remis_offer;
}

bool &Situation::has_remis_offer() {
	return _has_remis_offer;
}

bool Situation::has_game_ended() const {
	return _has_game_ended;
}

bool &Situation::has_game_ended() {
	return _has_game_ended;
}

Player Situation::winner() const {
	return _winner;
}

Player &Situation::winner() {
	return _winner;
}

// OPERATIONS

void Situation::action(const Action &a) {
	if (_has_game_ended)
		return;

	bool is_pawn = piece(a.src).type == TYPE_PAWN;
	bool is_capture = a.type == CAPTURE_PIECE || a.type == EN_PASSANT;

	if (a.type != DO_NOTHING) {
		Position::action(a);

		if (is_pawn || is_capture)
			_half_move_counter = 0;
		else
			++_half_move_counter;
	}

	_has_remis_offer = a.announcement == OFFER_REMIS;

	switch (a.announcement) {
	case NO_ANNOUNCEMENT:
	case OFFER_REMIS:
		break;

	case RESIGN: {
		Player opponent = static_cast<Player>((a.player + 1) % 2);
		_winner = opponent;
		_has_game_ended = true;
		break;
	}

	case ACCEPT_REMIS:
	case CLAIM_POSITION_REPETITION:
	case CLAIM_FIFTY_MOVES_RULE:
		_winner = PLAYER_NONE;
		_has_game_ended = true;
		break;
	} // switch announcement
}
