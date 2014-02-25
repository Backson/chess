#ifndef BOARD_HPP
#define BOARD_HPP

#include "Piece.hpp"
#include "vec.hpp"

static const int BOARD_WIDTH_DEFAULT  = 8;
static const int BOARD_HEIGHT_DEFAULT = 8;

typedef ltd::vec<int8, 2> Tile;

class Board {
public:
    Board();
	Board(int width, int height);
    Board(const Board &other);
    Board(const Piece[BOARD_HEIGHT_DEFAULT][BOARD_WIDTH_DEFAULT]);
    Board(const Piece **);
    Board(const Piece **, int width, int height);
    ~Board();
	
    Board &operator=(const Board &other);
	
    int8 width() const;
    int8 height() const;
	
	Piece piece(Tile tile) const;
	Piece &piece(Tile tile);
	
	Piece &operator[](Tile tile) const;
	Piece operator[](Tile tile);

    bool operator==(const Board &other) const;
    bool operator!=(const Board &other) const;
	
    void removePiece(Tile tile);
    void movePiece(Tile src, Tile dst);
	
    bool isInBound(Tile tile) const;

    static Board factoryStandard();
    static Board factoryEmpty();
	
	static const Tile INVALID_TILE;
private:
	int8 _width, _height;
    Piece** _pieces;
};

#endif // BOARD_HPP
