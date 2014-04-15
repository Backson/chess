#include "SpeedyBot.hpp"

#include "Rules.hpp"
#include "Position.hpp"

#include <cstdio>
#include <time.h>
#include <cfloat>

static float PLUS_INFINITY = 99999999.0f;
static float MINUS_INFINITY = -99999999.0f;
static float VERY_BAD = -9999999.0f;

static float PIECE_VALUES[6] = { 0, 9, 5, 3, 3, 1 };
static float PIECE_POS_RATINGS[6][8][8] = {
		{
				{1.0, 0.8, 0.3, 0.1, 0.0, 0.0, 0.0, 0.0},
				{1.0, 0.8, 0.3, 0.1, 0.0, 0.0, 0.0, 0.0},
				{1.0, 0.8, 0.3, 0.1, 0.0, 0.0, 0.0, 0.0},
				{0.7, 0.5, 0.3, 0.1, 0.0, 0.0, 0.0, 0.0},
				{0.7, 0.5, 0.3, 0.1, 0.0, 0.0, 0.0, 0.0},
				{1.0, 0.8, 0.3, 0.1, 0.0, 0.0, 0.0, 0.0},
				{1.0, 0.8, 0.3, 0.1, 0.0, 0.0, 0.0, 0.0},
				{1.0, 0.8, 0.3, 0.1, 0.0, 0.0, 0.0, 0.0},
		}, {
				{0.0, 0.1, 0.2, 0.3, 0.4, 0.4, 0.3, 0.2},
				{0.1, 0.2, 0.3, 0.4, 0.5, 0.4, 0.3, 0.2},
				{0.2, 0.3, 0.4, 0.6, 0.8, 0.7, 0.3, 0.2},
				{0.3, 0.4, 0.5, 0.9, 1.0, 0.8, 0.5, 0.2},
				{0.3, 0.4, 0.5, 0.9, 1.0, 0.8, 0.5, 0.2},
				{0.2, 0.3, 0.4, 0.6, 0.8, 0.7, 0.3, 0.2},
				{0.1, 0.2, 0.3, 0.4, 0.5, 0.4, 0.3, 0.2},
				{0.0, 0.1, 0.2, 0.3, 0.4, 0.4, 0.3, 0.2},
		}, {
				{0.2, 0.2, 0.3, 0.4, 0.4, 0.3, 0.3, 0.3},
				{0.4, 0.2, 0.2, 0.2, 0.1, 0.2, 0.3, 0.3},
				{0.6, 0.3, 0.2, 0.2, 0.1, 0.2, 0.3, 0.3},
				{1.0, 0.9, 0.5, 0.5, 0.4, 0.3, 0.3, 0.3},
				{1.0, 0.9, 0.5, 0.5, 0.4, 0.3, 0.3, 0.3},
				{0.6, 0.3, 0.2, 0.2, 0.1, 0.2, 0.3, 0.3},
				{0.4, 0.2, 0.2, 0.2, 0.1, 0.2, 0.3, 0.3},
				{0.2, 0.2, 0.3, 0.4, 0.4, 0.3, 0.3, 0.3},
		}, {
				{0.0, 0.1, 0.3, 0.4, 0.3, 0.2, 0.1, 0.0},
				{0.0, 0.2, 0.3, 0.5, 0.4, 0.3, 0.2, 0.0},
				{0.1, 0.2, 0.4, 0.8, 0.6, 0.4, 0.2, 0.1},
				{0.1, 0.3, 0.7, 1.0, 0.8, 0.5, 0.3, 0.1},
		}, {
				{0.0, 0.0, 0.1, 0.1, 0.1, 0.1, 0.0, 0.0},
				{0.0, 0.1, 0.2, 0.3, 0.3, 0.2, 0.1, 0.0},
				{0.1, 0.2, 0.4, 0.5, 0.5, 0.4, 0.2, 0.1},
				{0.1, 0.3, 0.7, 1.0, 1.0, 0.7, 0.3, 0.1},
				{0.1, 0.3, 0.7, 1.0, 1.0, 0.7, 0.3, 0.1},
				{0.1, 0.2, 0.4, 0.5, 0.5, 0.4, 0.2, 0.1},
				{0.0, 0.1, 0.2, 0.3, 0.3, 0.2, 0.1, 0.0},
				{0.0, 0.0, 0.1, 0.1, 0.1, 0.1, 0.0, 0.0},
		}, {
				{0.0, 0.0, 0.1, 0.2, 0.4, 0.7, 1.0, 0.0},
				{0.0, 0.0, 0.1, 0.3, 0.5, 0.8, 1.0, 0.0},
				{0.0, 0.0, 0.2, 0.5, 0.6, 0.9, 1.0, 0.0},
				{0.0, 0.0, 0.3, 0.6, 0.7, 0.9, 1.0, 0.0},
				{0.0, 0.0, 0.3, 0.6, 0.7, 0.9, 1.0, 0.0},
				{0.0, 0.0, 0.2, 0.5, 0.6, 0.9, 1.0, 0.0},
				{0.0, 0.0, 0.1, 0.3, 0.5, 0.8, 1.0, 0.0},
				{0.0, 0.0, 0.1, 0.2, 0.4, 0.7, 1.0, 0.0},
		}
};

SpeedyBot::SpeedyBot() :
	Bot(), _max_depth(3)
{
	// nothing
}

SpeedyBot::SpeedyBot(int maxDepth) :
	Bot(), _max_depth(maxDepth)
{
	// nothing
}

SpeedyBot::SpeedyBot(const Situation &situation, int maxDepth) :
	Bot(situation), _max_depth(maxDepth)
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
	float bestRating = rate_game(_max_depth, MINUS_INFINITY, PLUS_INFINITY, 0, position, &action);

	printf("bestRating: %.0f\n", bestRating);

	return action;
}

float SpeedyBot::rate_game(int depth, float alpha, float beta, int dist, Position &position, Action *outAction) {
	Rules rules;
	std::vector<Action> actions = rules.getAllLegalMoves(position);
	if(actions.size() == 0) {
		if(rules.isPlayerInCheck(position, position.active_player()))
			return VERY_BAD + dist;
		else
			return 0;
	}
	float bestRating = MINUS_INFINITY;
	for (auto iter = actions.begin(); iter != actions.end(); ++iter) {
		Delta delta;
		position.action(*iter, &delta);
		float rating;
		if (depth == 0)
			rating = -rate_game_flat(dist, position);
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
	float material = 0;
	float posRating = 0;
	int numPieces = 0;
	for (Coord y = 0; y < position.height(); ++y)
	for (Coord x = 0; x < position.width(); ++x) {
		Piece p = position[Tile(x,y)];

		if(p.type == TYPE_NONE)
			continue;

		float factor = 1.0;
		if(p.player != position.active_player())
			factor = -1.0;

		int relative_y = y;
		if(p.player == PLAYER_BLACK)
			relative_y = position.width() - y - 1;

		material += PIECE_VALUES[p.type] * factor;
		posRating += PIECE_POS_RATINGS[p.type][x][relative_y] * factor;
		numPieces++;
	}

	Rules rules;
	if(rules.getAllLegalMoves(position).size() == 0) {
		if(rules.isPlayerInCheck(position, position.active_player()))
			return VERY_BAD + dist;
		else
			return 0;
	}
	return material + posRating / numPieces;
}
