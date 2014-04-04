#ifndef ZOBRIST_HPP
#define ZOBRIST_HPP

#include "stdtypes.hpp"
#include "Action.hpp"

uint32 zobrist_piece_tile(Piece piece, Tile tile, int width);

uint32 zobrist_player();

uint32 zobrist_file(int file);

uint32 zobrist_castling(Player player, CastlingType type);

#endif // ZOBRIST_HPP
