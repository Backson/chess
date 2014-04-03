#ifndef BOT_HPP
#define BOT_HPP

#include "Game.hpp"

class Bot {
public:
    Bot();
    Bot(const Situation &);

	void seek(int);
	void pop();

    void reset();
    void reset(const Situation &);

    const Game &game() const;

    virtual void update(Action);
    virtual Action next_action() = 0;

protected:
    Game _game;
};

#endif // BOT_HPP
