#include "SpeedyBot.hpp"

#include "Rules.hpp"

#include <cstdio>
#include <time.h>
#include <cfloat>

static float PLUS_INFINITY = 99999999.0f;
static float MINUS_INFINITY = -99999999.0f;
static float VERY_BAD = -9999999.0f;

SpeedyBot::SpeedyBot() :
	Bot()
{
	// nothing
}

SpeedyBot::SpeedyBot(const Situation &situation) :
	Bot(situation)
{
	// nothing
}

/* // only override this, if you need to
void SpeedyBot::update(Action a) {
	Bot::update(a);

	// do your own stuff here
}
*/

Action SpeedyBot::next_action() {
	Action action;
	float bestRating = rate_game(3, MINUS_INFINITY, PLUS_INFINITY, &action);

	printf("bestRating: %.0f\n", bestRating);

	return action;
}

float SpeedyBot::rate_game(int depth, float alpha, float beta, Action *outAction) {
	Rules rules;
	std::vector<Action> actions = rules.getAllLegalMoves(_game.current_situation());
	float bestRating = MINUS_INFINITY;
	for (auto iter = actions.begin(); iter != actions.end(); ++iter) {
		_game.action(*iter);
		float rating;
		if (depth == 0)
			rating = -rate_game_flat();
		else
			rating = -rate_game(depth - 1, -beta, -bestRating);
		_game.pop();
		if (rating > bestRating) {
			bestRating = rating;
			if(outAction)
				*outAction = *iter;
			if(rating >= beta)
				return bestRating;
		}
	}

	return bestRating;
}

float SpeedyBot::rate_game_flat() {
	float rating = 0;
	const Situation &situation = _game.current_situation();
	int pawnSum = 0;
	for (Coord y = 0; y < situation.height(); ++y)
	for (Coord x = 0; x < situation.width(); ++x) {
		Piece p = situation[Tile(x,y)];

		if(p.type == TYPE_NONE)
			continue;

		int factor = 1;
		if(p.player != situation.active_player())
			factor = -1;

		switch(p.type) {
		case TYPE_QUEEN:
			rating += 9 * factor;
			break;
		case TYPE_ROOK:
			rating += 5 * factor;
			break;
		case TYPE_BISHOP:
			rating += 3 * factor;
			break;
		case TYPE_KNIGHT:
			rating += 3 * factor;
			break;
		case TYPE_PAWN:
			if(p.player == PLAYER_BLACK)
				pawnSum += (situation.width() - y - 1) * factor;
			else
				pawnSum += y * factor;
			rating += factor;
			break;
		default:
			break;
		}
	}

	Rules rules;
	int numMoves = rules.getAllLegalMoves(_game.current_situation()).size();
	if(numMoves == 0)
		return VERY_BAD;
	return rating + numMoves / (64.0f * 64.0f) + pawnSum / (56.0f * 64.0f * 64.0f);
}
