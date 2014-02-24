/** @file Piece.h
 *
 */

#ifndef Piece_h
#define Piece_h

#include "types.h"

class Piece
{
public:
    Piece();
    Piece(const Piece& other);
    Piece(Type type, Player player);
    explicit Piece(BitwisePiece piece);
    Piece& operator=(const Piece& other);
    Piece& operator=(BitwisePiece piece);

    bool operator==(const Piece& other) const;
    bool operator!=(const Piece& other) const;

    operator BitwisePiece() const;

    Player getPlayer() const;
    Type getType() const;

    bool isNone() const;

    static const Piece NONE;

private:
    #if BITWISE_TYPES
      BitwisePiece piece;
    #else
      Type type;
      Player player;
    #endif

    Piece(Type); // disallow inappropriate implicit cast
    Piece(Player); // disallow inappropriate implicit cast
};

#endif // Piece_h
