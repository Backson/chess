/** @file Square.cpp
 *
 */

#include "Square.h"

Square::Square()
  #if BITWISE_TYPES
    : square(~0)
  #else
    : rank(-1), file(-1)
  #endif
{
    // nothing
}
Square::Square(const Square& other)
  #if BITWISE_TYPES
    : square(other.square)
  #else
    : rank(other.rank), file(other.file)
  #endif
{
    // nothing
}
Square::Square(BitwiseSquare square)
  #if BITWISE_TYPES
    : square(square)
  #else
    : rank(get_rank(square)), file(get_file(square))
  #endif
{

}
Square::Square(Coord rank, Coord file)
  #if BITWISE_TYPES
    : square(from_coords(rank, file))
  #else
    : rank(rank), file(file)
  #endif
{
    // nothing
}
Square& Square::operator=(const Square& other)
{
    #if BITWISE_TYPES
        this->square = other.square;
    #else
        this->rank = other.rank;
        this->file = other.file;
    #endif
    return *this;
}
Square& Square::operator=(BitwiseSquare square)
{
    #if BITWISE_TYPES
        this->square = square;
    #else
        this->rank = get_rank(square);
        this->file = get_file(square);
    #endif
    return *this;
}

bool Square::operator==(const Square& other) const
{
    if ( !this->isValid() && !other.isValid() )
        return true;

    #if BITWISE_TYPES
        return this->square == other.square;
    #else
        return rank == other.rank && file == other.file;
    #endif
}
bool Square::operator!=(const Square& other) const
{
    if ( !this->isValid() && !other.isValid() )
        return false;

    #if BITWISE_TYPES
        return this->square != other.square;
    #else
        return rank != other.rank || file != other.file;
    #endif
}

Square::operator BitwiseSquare() const
{
    #if BITWISE_TYPES
        return square;
    #else
        return from_coords(rank, file);
    #endif
}

Coord Square::getRank() const
{
    #if BITWISE_TYPES
        return get_rank(square);
    #else
        return rank;
    #endif
}
Coord Square::getFile() const
{
    #if BITWISE_TYPES
        return get_file(square);
    #else
        return file;
    #endif
}

bool Square::isValid() const
{
    Coord rank = getRank();
    Coord file = getFile();
    bool result = (rank != -1) && (file != -1);
    return result;
}
