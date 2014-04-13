#include "RandomBot.hpp"

#include "Rules.hpp"

#include <cstdio>
#include <time.h>

static uint32 get_unique_seed() {
	std::random_device rd;
	if (rd.entropy() > 0) {
		uint32 seed = rd();
		printf("unique seed generated: 0x%04X %04X\n", seed >> 16, seed & 0xFFFF);
		return seed;
	} else {
		uint32 seed = time(NULL);
		printf("pseudo unique seed generated: 0x%04X %04X\n", (seed >> 16) & 0xFFFF, seed & 0xFFFF);
		return seed;
	}
}

RandomBot::RandomBot() :
	Bot(),
	_random(get_unique_seed())
{
	// nothing
}
RandomBot::RandomBot(uint32 seed) :
	Bot(),
	_random(seed)
{
	// nothing
}

RandomBot::RandomBot(const Situation &situation) :
	Bot(situation),
	_random(get_unique_seed())
{
	// nothing
}
RandomBot::RandomBot(const Situation &situation, uint32 seed) :
	Bot(situation),
	_random(seed)
{
	// nothing
}

/* // only override this, if you need to
void RandomBot::update(Action a) {
	Bot::update(a);

	// do your own stuff here
}
*/

Action RandomBot::next_action() {
	Rules rules;
	std::vector<Action> actions = rules.getAllLegalMoves(_game);
	if (actions.size() == 0)
		return {_game.current_situation().active_player(),
				DO_NOTHING, Board::INVALID_TILE, Board::INVALID_TILE, TYPE_NONE};
	std::uniform_int_distribution<> dist(0, actions.size() - 1);
	int index = dist(_random);
	printf("bot is choosing from %u moves... %d!\n", (uint) actions.size(), index);
	for (auto iter = actions.begin(); iter != actions.end(); ++iter) {
		if (iter != actions.begin()) {
			printf(" ");
		}
		Action a = *iter;
		printf("%c%c->%c%c", 'A'+a.src[0], '1'+a.src[1], 'A'+a.dst[0], '1'+a.dst[1]);
	}
	printf("\n");
	return actions[index];
}
