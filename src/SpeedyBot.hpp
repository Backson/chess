#ifndef SPEEDY_BOT_HPP
#define SPEEDY_BOT_HPP

#include "Bot.hpp"

#include <random>

class SpeedyBot :
    public Bot
{
public:
	SpeedyBot();
	SpeedyBot(uint32);
	SpeedyBot(const Situation &);
	SpeedyBot(const Situation &, uint32);

	//virtual void update(Action) override;
	virtual Action next_action() override;

private:
	float rate_game(int, Action * = 0);
	float rate_game_flat();
};

#endif // SPEEDY_BOT_HPP
