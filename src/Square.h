/** @file Square.h
 *
 */

#ifndef Square_h
#define Square_h

#include "types.h"

class Square
{
public:
    // LIFECYCLE
    // std ctor
    Square();
    // copy ctor
    Square(const Square& other);
    // conversion ctors
    explicit Square(BitwiseSquare square);
    Square(Coord rank, Coord file);

    // ASSIGNMENT
    Square& operator=(const Square& other);
    Square& operator=(BitwiseSquare square);

    // OPERATORS
    bool operator==(const Square& other) const;
    bool operator!=(const Square& other) const;

    // CONVERSION
    operator BitwiseSquare() const;

    Square(Coord); // disallow inappropriate implicit cast

    // ACCESS
    Coord getRank() const;
    Coord getFile() const;

    bool isValid() const;

private:
    #if BITWISE_TYPES
      BitwiseSquare square;
    #else
      Coord rank, file;
    #endif
};

#endif // Square_h
