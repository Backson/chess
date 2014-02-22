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

#define DEFINE_PIECE_SHORTCUTS \
    const Piece K = Piece(PLAYER_WHITE | TYPE_KING); \
    const Piece Q = Piece(PLAYER_WHITE | TYPE_QUEEN); \
    const Piece R = Piece(PLAYER_WHITE | TYPE_ROOK); \
    const Piece B = Piece(PLAYER_WHITE | TYPE_BISHOP); \
    const Piece N = Piece(PLAYER_WHITE | TYPE_KNIGHT); \
    const Piece P = Piece(PLAYER_WHITE | TYPE_PAWN); \
    \
    const Piece k = Piece(PLAYER_BLACK | TYPE_KING); \
    const Piece q = Piece(PLAYER_BLACK | TYPE_QUEEN); \
    const Piece r = Piece(PLAYER_BLACK | TYPE_ROOK); \
    const Piece b = Piece(PLAYER_BLACK | TYPE_BISHOP); \
    const Piece n = Piece(PLAYER_BLACK | TYPE_KNIGHT); \
    const Piece p = Piece(PLAYER_BLACK | TYPE_PAWN); \
    \
    const Piece _ = Piece(PLAYER_WHITE | TYPE_NONE)

#endif // Piece_h
