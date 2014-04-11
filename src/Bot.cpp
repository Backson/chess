#include "Bot.hpp"

Bot::Bot(const Situation &situation) :
    _game(situation)
{
    // nothing
}

void Bot::seek(int number) {
    _game.seek(number);
}

void Bot::pop() {
    _game.pop();
}

void Bot::reset() {
    Situation situation;
    reset(situation);
}

void Bot::reset(const Situation &situation) {
    _game.reset(situation);
}

const Game &Bot::game() const {
    return _game;
}

void Bot::update(Action a) {
    _game.action(a);
}

