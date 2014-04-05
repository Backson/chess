#ifndef BOT_HPP
#define BOT_HPP

#include "Game.hpp"

class Bot {
public:
    Bot() = default;
    Bot(const Situation &);
    virtual ~Bot() = default;

	void seek(int);
	void pop();

    void reset();
    void reset(const Situation &);

    const Game &game() const;

    virtual void update(Action);
    virtual Action next_action() = 0;

private:
    Game _game;
};

#endif // BOT_HPP
