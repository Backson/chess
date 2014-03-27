#ifndef SITUATION_HPP
#define SITUATION_HPP

#include "Position.hpp"

class Situation :
    public Position
{
public:
    // LIFECYCLE
    using Position::Position;

    // ACCESS
    int half_move_counter() const;
    int &half_move_counter();

    bool has_remis_offer() const;
    bool &has_remis_offer();


private:
    int _half_move_counter = 0;
    bool _has_remis_offer = false;
};

#endif // SITUATION_HPP
