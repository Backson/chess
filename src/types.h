/** @file types.h
 *
 */

#ifndef types_h
#define types_h

// With BITWISE_TYPES defined to 1, types are packed into small structures,
// sometimes even multiple enums in one char. uncomment 0 to prevent this
// and save the enum members directly (memory intense!)
// either way, implicit conversions are done
// (but might be faster or slower, which is ok for O(n) algorithms)
//#define BITWISE_TYPES   0
#define BITWISE_TYPES   1

#include "constants.h"

#if (BOARD_WIDTH > 8) || (BOARD_HEIGHT > 8)
  #if BITWISE_TYPES
    #error BITWISE_TYPES for Square not supported for too large board sizes
  #else
    #warning BITWISE_TYPES for Square not supported for too large board sizes
    #define PARTIAL_BITWISE_SUPPORT
  #endif
#endif

enum Type
{
    TYPE_NONE      = 0x0,
    TYPE_KING      = 0x1,
    TYPE_QUEEN     = 0x2,
    TYPE_ROOK      = 0x3,
    TYPE_BISHOP    = 0x4,
    TYPE_KNIGHT    = 0x5,
    TYPE_PAWN      = 0x6,
};

enum Player
{
    PLAYER_WHITE    = 0x0,
    PLAYER_BLACK    = 0x8,
};

typedef unsigned char BitwisePiece;
#define PLAYER_MASK ( PLAYER_WHITE | PLAYER_BLACK )
#define TYPE_MASK ( TYPE_KING | TYPE_QUEEN | TYPE_ROOK | TYPE_BISHOP | \
                    TYPE_KNIGHT | TYPE_PAWN )

typedef signed char Coord;
typedef unsigned char BitwiseSquare;
#define RANK_MASK       0x0F
#define FILE_MASK       0xF0
#define RANK_INVALID    0x08
#define FILE_INVALID    0x80
#define RANK_SHIFT      0
#define FILE_SHIFT      4

inline BitwiseSquare from_coords(Coord rank, Coord file)
{
    BitwiseSquare unsigned_rank;
    BitwiseSquare unsigned_file;

    if (rank < 0)
    {
        unsigned_rank = ~rank;
        unsigned_rank <<= RANK_SHIFT;
        unsigned_rank |= RANK_INVALID;
        unsigned_rank &= RANK_MASK;
    }
    else
    {
        unsigned_rank = rank;
        unsigned_rank <<= RANK_SHIFT;
        unsigned_rank &= RANK_MASK;
    }

    if (file < 0)
    {
        unsigned_file = ~file;
        unsigned_file <<= FILE_SHIFT;
        unsigned_file |= FILE_INVALID;
        unsigned_file &= FILE_MASK;
    }
    else
    {
        unsigned_file = file;
        unsigned_file <<= FILE_SHIFT;
        unsigned_file &= FILE_MASK;
    }

    BitwiseSquare result = unsigned_rank | unsigned_file;
    return result;
}

inline Coord get_rank(BitwiseSquare square)
{
    Coord rank;
    if (square & RANK_INVALID)
    {
        square &= RANK_MASK;
        square ^= RANK_INVALID;
        square >>= RANK_SHIFT;
        rank = square;
        rank = ~rank;
    }
    else
    {
        square &= RANK_MASK;
        square >>= RANK_SHIFT;
        rank = square;
    }

    return rank;
}

inline Coord get_file(BitwiseSquare square)
{
    Coord file;
    if (square & FILE_INVALID)
    {
        square &= FILE_MASK;
        square ^= FILE_INVALID;
        square >>= FILE_SHIFT;
        file = square;
        file = ~file;
    }
    else
    {
        square &= FILE_MASK;
        square >>= FILE_SHIFT;
        file = square;
    }

    return file;
}

#endif // types_h
