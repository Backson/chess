#ifndef SITUATION_HPP
#define SITUATION_HPP

#include "Position.hpp"

class Situation :
    public Position
{
public:

private:
    int _half_move_counter;
    bool _has_remis_offer;
};

#endif // SITUATION_HPP
