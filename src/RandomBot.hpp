#ifndef RANDOM_BOT_HPP
#define RANDOM_BOT_HPP

#include "Bot.hpp"

#include <random>

class RandomBot :
    public Bot
{
public:
    RandomBot();
    RandomBot(uint32);
    RandomBot(const Situation &);
    RandomBot(const Situation &, uint32);

    //virtual void update(Action) override;
    virtual Action next_action() override;

private:
    std::mt19937 _random;
};

#endif // RANDOM_BOT_HPP
