/** @file Piece.cpp
 *
 */

#include "Piece.h"

Piece::Piece()
  #if BITWISE_TYPES
    : piece(0)
  #else
    : type(TYPE_NONE), player(PLAYER_WHITE)
  #endif
{
    // nothing
}

Piece::Piece(Type type, Player player)
  #if BITWISE_TYPES
    : piece(type | player)
  #else
    : type(type), player(player)
  #endif
{
    // nothing
}

Piece::Piece(BitwisePiece piece)
  #if BITWISE_TYPES
    : piece(piece)
  #endif
{
  #if BITWISE_TYPES
    // nothing
  #else
    this->player = static_cast<Player>(piece & PLAYER_MASK);
    this->type = static_cast<Type>(piece & TYPE_MASK);
  #endif
}

Piece::Piece(const Piece& other)
  #if BITWISE_TYPES
    : piece(other.piece)
  #else
    : type(other.type), player(other.player)
  #endif
{
    // nothing
}

Piece& Piece::operator=(const Piece& rhs)
{
    #if BITWISE_TYPES
        this->piece = rhs.piece;
    #else
        this->type = rhs.type;
        this->player = rhs.player;
    #endif
    return *this;
}

bool Piece::operator==(const Piece& other) const
{
    if (this->isNone() && other.isNone())
        return true;
    #if BITWISE_TYPES
        return this->piece == other.piece;
    #else
        return this->type == other.type && this->player == other.player;
    #endif
}

bool Piece::operator!=(const Piece& other) const
{
    if (this->isNone() && other.isNone())
        return false;
    #if BITWISE_TYPES
        return this->piece != other.piece;
    #else
        return this->type != other.type || this->player != other.player;
    #endif
}

Piece::operator BitwisePiece() const
{
    #if BITWISE_TYPES
        return this->piece;
    #else
        return this->type | this->player;
    #endif
}

Piece& Piece::operator=(BitwisePiece piece)
{
  #if BITWISE_TYPES
    this->piece = piece;
  #else
    player = static_cast<Player>(piece & PLAYER_MASK);
    type = static_cast<Type>(piece & TYPE_MASK);
  #endif
    return *this;
}

Player Piece::getPlayer() const
{
  #if BITWISE_TYPES
    return static_cast<Player>(piece & PLAYER_MASK);
  #else
    return player;
  #endif
}

Type Piece::getType() const
{
  #if BITWISE_TYPES
    return static_cast<Type>(piece & TYPE_MASK);
  #else
    return type;
  #endif
}

bool Piece::isNone() const
{
    return getType() == TYPE_NONE;
}

const Piece Piece::NONE = Piece(0);
