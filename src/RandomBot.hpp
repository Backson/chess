#ifndef RANDOM_BOT_HPP
#define RANDOM_BOT_HPP

#include "Game.hpp"

#include <random>

class RandomBot {
public:
    RandomBot();
    RandomBot(uint32);

    void reset();
    void reset(const Situation &);

    void update(Action);
    Action getAction();

private:
    Game _game;
    std::mt19937 _random;
};

#endif // RANDOM_BOT_HPP
