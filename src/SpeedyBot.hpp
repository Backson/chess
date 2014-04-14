#ifndef SPEEDY_BOT_HPP
#define SPEEDY_BOT_HPP

#include "Bot.hpp"

#include <random>

class SpeedyBot :
    public Bot
{
public:
	SpeedyBot();
	SpeedyBot(int);
	SpeedyBot(const Situation &, int);

	//virtual void update(Action) override;
	virtual Action next_action() override;

private:
	float rate_game(int, float, float, int, Position &, Action * = 0);
	float rate_game_flat(int, const Position &);

	int _max_depth;
};

#endif // SPEEDY_BOT_HPP
