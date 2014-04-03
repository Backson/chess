#ifndef RANDOM_BOT_HPP
#define RANDOM_BOT_HPP

#include "Bot.hpp"

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
	int rate_game(int depth, const Situation &, Action *outAction = 0);
	int rate_game(int depth, Action *outAction = 0);
	int rate_game_flat(const Situation &);
};

#endif // RANDOM_BOT_HPP
