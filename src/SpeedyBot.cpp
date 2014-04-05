#include "SpeedyBot.hpp"

#include "Rules.hpp"

#include <cstdio>
#include <time.h>
#include <climits>

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
    int bestRating = rate_game(2, &action);

	printf("bestRating: %d\n", bestRating);

    return action;
}

int SpeedyBot::rate_game(int depth, const Situation &situation, Action *outAction) {

    Rules rules;
    std::vector<Action> actions = rules.getAllLegalMoves(situation);
    int bestRating = INT_MIN;
    for (auto iter = actions.begin(); iter != actions.end(); ++iter) {
    	//update(*iter);
    	Situation new_situation(situation);
    	int rating;
    	if (depth == 0)
    		rating = -rate_game_flat(new_situation);
    	else
    		rating = -rate_game(depth - 1, new_situation);
    	if (rating > bestRating) {
    		bestRating = rating;
    		if(outAction)
    			*outAction = *iter;
    	}
    	//pop();
    }

    return bestRating;
}

int SpeedyBot::rate_game(int depth, Action *outAction) {
    const Situation &situation = game().current_situation();
    return rate_game(depth, situation, outAction);
}

int SpeedyBot::rate_game_flat(const Situation &situation) {
	int rating = 0;
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
            rating += factor;
            break;
        default:
            break;
    	}
    }

	return rating;
}
