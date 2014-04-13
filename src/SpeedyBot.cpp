#include "SpeedyBot.hpp"

#include "Rules.hpp"
#include "Position.hpp"

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
	Position position = _game.current_situation();
	float bestRating = rate_game(4, MINUS_INFINITY, PLUS_INFINITY, 0, position, &action);

	printf("bestRating: %.0f\n", bestRating);

	return action;
}

float SpeedyBot::rate_game(int depth, float alpha, float beta, int dist, Position &position, Action *outAction) {
	Rules rules;
	std::vector<Action> actions = rules.getAllLegalMoves(position);
	if(actions.size() == 0)
		return VERY_BAD + dist;
	float bestRating = MINUS_INFINITY;
	for (auto iter = actions.begin(); iter != actions.end(); ++iter) {
		Delta delta;
		position.action(*iter, &delta);
		float rating;
		if (depth == 0)
			rating = -rate_game_flat(dist + 1, position);
		else
			rating = -rate_game(depth - 1, -beta, -bestRating, dist + 1, position);
		position.apply(delta);
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

float SpeedyBot::rate_game_flat(int dist, const Position &position) {
	float rating = 0;
	int pawnSum = 0;
	for (Coord y = 0; y < position.height(); ++y)
	for (Coord x = 0; x < position.width(); ++x) {
		Piece p = position[Tile(x,y)];

		if(p.type == TYPE_NONE)
			continue;

		int factor = 1;
		if(p.player != position.active_player())
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
				pawnSum += (position.width() - y - 1) * factor;
			else
				pawnSum += y * factor;
			rating += factor;
			break;
		default:
			break;
		}
	}

	Rules rules;
	int numMoves = rules.getAllLegalMoves(position).size();
	if(numMoves == 0)
		return VERY_BAD + dist;
	return rating + numMoves / (64.0f * 64.0f) + pawnSum / (56.0f * 64.0f * 64.0f);
}
