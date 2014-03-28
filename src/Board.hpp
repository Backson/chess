#ifndef BOARD_HPP
#define BOARD_HPP

#include "Piece.hpp"
#include "vec.hpp"

// when sizes are omitted in construction, these default sizes are used
// either both or none of the dimensions are omitted
static const int BOARD_WIDTH_DEFAULT  = 8;
static const int BOARD_HEIGHT_DEFAULT = 8;

// used to identify locations on the board in a universal way
typedef int8 Coord;
typedef ltd::vec<Coord, 2> Tile;

class Board {
public:

	static const int ROW_MAJOR    = 0; // pseudo flag, gets ignored
	static const int COLUMN_MAJOR = 1;
	static const int FLIP_X       = 2;
	static const int FLIP_Y       = 4;

	// LIFECYCLE
	~Board();
	Board(const Board &);
	Board(Board &&);
	Board &operator = (const Board &);
	Board &operator = (Board &&);
	Board();
	Board(Coord width, Coord height);
	Board(const Piece *, int flags = 0);
	Board(const Piece **, int flags = 0);
	Board(const Piece *, Coord width, Coord height, int flags = 0);
	Board(const Piece **, Coord width, Coord height, int flags = 0);
	Board(const Piece[BOARD_HEIGHT_DEFAULT][BOARD_WIDTH_DEFAULT]);

	// ACCESS
	Coord width() const;
	Coord height() const;

	Piece piece(Tile tile) const;
	Piece &piece(Tile tile);

	bool isInBound(Tile tile) const;

	// OPERATORS
	Piece operator [] (Tile tile) const;
	Piece &operator [] (Tile tile);

	bool operator == (const Board &rhs) const;
	bool operator != (const Board &rhs) const;

	// OPERATIONS
	void resize();

	void removePiece(Tile tile);
	void movePiece(Tile src, Tile dst);

	// MISC

	static Board factoryStandard();
	static Board factoryEmpty();

	static const Tile INVALID_TILE;

private:
	Coord _width = BOARD_WIDTH_DEFAULT;
	Coord _height = BOARD_HEIGHT_DEFAULT;
	Piece** _pieces = nullptr;
};

#endif // BOARD_HPP
