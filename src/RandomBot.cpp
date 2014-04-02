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

RandomBot::RandomBot() : _random(get_unique_seed()) {
    // nothing
}
RandomBot::RandomBot(uint32 seed) : _random(seed) {
    // nothing
}

void RandomBot::reset() {
    Situation situation;
    reset(situation);
}
void RandomBot::reset(const Situation &situation) {
    _game.reset(situation);
}

void RandomBot::update(Action a) {
    _game.action(a);
}

Action RandomBot::getAction() {
    Rules rules;
    std::vector<Action> actions = rules.getAllLegalMoves(_game);
    std::uniform_int_distribution<> dist(0, actions.size() - 1);
    int index = dist(_random);
    printf("bot is choosing from %d moves... %d!\n", actions.size(), index);
    return actions[index];
}
