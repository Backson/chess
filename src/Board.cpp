#include "Board.hpp"

using std::shared_ptr;
using std::make_shared;

// LIFECYCLE

Board::~Board() {
	if (_pieces) {
		for (Coord y = 0; y < _height; ++y)
			delete[] _pieces[y];
		delete[] _pieces;
	}
}

Board::Board(const Board &other) :
	_width(other._width),
	_height(other._height)
{
	_pieces = new Piece*[_height];
	for (Coord y = 0; y < _height; ++y) {
		_pieces[y] = new Piece[_width];

		for (Coord x = 0; x < _width; ++x) {
			_pieces[y][x] = other._pieces[y][x];
		}
	}
}

Board::Board(Board &&other) :
	_width(other._width),
	_height(other._height),
	_pieces(other._pieces)
{
	other._pieces = nullptr;
}

Board &Board::operator = (const Board &rhs) {
	if (this == &rhs) return *this; // self assignment

	bool must_resize = _width != rhs._width || _height != rhs._height;
	if (must_resize) {
		if (_pieces) {
			for (Coord y = 0; y < _height; ++y)
				delete[] _pieces[y];
			delete[] _pieces;
		}

		_width = rhs._width;
		_height = rhs._height;

		_pieces = new Piece*[_height];
		for (Coord y = 0; y < _height; ++y) {
			_pieces[y] = new Piece[_width];
		}
	}

	for (Coord y = 0; y < _height; ++y) {
		for (Coord x = 0; x < _width; ++x) {
			this->_pieces[y][x] = rhs._pieces[y][x];
		}
	}
	return *this;
}

Board &Board::operator = (Board &&rhs) {
	if (this == &rhs) return *this; // self assignment

	if (_pieces) {
		for (Coord y = 0; y < _height; ++y)
			delete[] _pieces[y];
		delete[] _pieces;
	}

	_width = rhs._width;
	_height = rhs._height;
	_pieces = rhs._pieces;
	rhs._pieces = nullptr;

	return *this;
}

Board::Board() :
	Board(BOARD_WIDTH_DEFAULT, BOARD_HEIGHT_DEFAULT)
{
	// nothing
}

Board::Board(Coord width, Coord height) :
	_width(width),
	_height(height)
{
	_pieces = new Piece*[_height];
	for (Coord y = 0; y < _height; ++y) {
		_pieces[y] = new Piece[_width];

		for (Coord x = 0; x < _width; ++x) {
			_pieces[y][x] = Piece::NONE;
		}
	}
}

Board::Board(const Piece *pieces, int flags) :
	Board(pieces, BOARD_WIDTH_DEFAULT, BOARD_HEIGHT_DEFAULT, flags)
{
	// nothing
}

Board::Board(const Piece **pieces, int flags) :
	Board(pieces, BOARD_WIDTH_DEFAULT, BOARD_HEIGHT_DEFAULT, flags)
{
	// nothing
}

Board::Board(const Piece *pieces, Coord width, Coord height, int flags) :
	_width(width),
	_height(height)
{
	_pieces = new Piece*[_height];
	for (Coord y = 0; y < _height; ++y) {
		_pieces[y] = new Piece[_width];

		for (Coord x = 0; x < _width; ++x) {
			Coord xx = flags & FLIP_X ? _width - x - 1: x;
			Coord yy = flags & FLIP_Y ? _height - y - 1: y;
			bool flip = flags & COLUMN_MAJOR;
			(flip ? _pieces[xx][yy] : _pieces[yy][xx]) = *(pieces++);
		}
	}
}

Board::Board(const Piece **pieces, Coord width, Coord height, int flags) :
	_width(width),
	_height(height)
{
	_pieces = new Piece*[_height];
	for (Coord y = 0; y < _height; ++y) {
		_pieces[y] = new Piece[_width];

		for (Coord x = 0; x < _width; ++x) {
			Coord xx = flags & FLIP_X ? _width - x - 1: x;
			Coord yy = flags & FLIP_Y ? _height - y - 1: y;
			bool flip = flags & COLUMN_MAJOR;
			(flip ? _pieces[xx][yy] : _pieces[yy][xx]) = pieces[y][x];
		}
	}
}

Board::Board(const Piece pieces[BOARD_HEIGHT_DEFAULT][BOARD_WIDTH_DEFAULT]) :
	Board(&pieces[0][0], BOARD_WIDTH_DEFAULT, BOARD_HEIGHT_DEFAULT)
{
	// nothing
}

// ACCESS

Coord Board::width() const {
	return _width;
}

Coord Board::height() const {
	return _height;
}

Piece Board::piece(Tile tile) const {
	return _pieces[tile[1]][tile[0]];
}

Piece &Board::piece(Tile tile) {
	return _pieces[tile[1]][tile[0]];
}

// OPERATORS

bool Board::operator == (const Board &rhs) const {
	if (this == &rhs) return true; // self-comparison

	// handle boards of different sizes
	if (_width != rhs._width) return false;
	if (_height != rhs._height) return false;

	// compare per piece
	for (Coord y = 0; y < _height; ++y) {
		for (Coord x = 0; x < _width; ++x) {
			if (_pieces[y][x] != rhs._pieces[y][x])
				return false;
		}
	}

	return true;
}

bool Board::operator != (const Board &rhs) const {
	return !operator == (rhs);
}

void Board::removePiece(Tile tile) {
	piece(tile) = Piece::NONE;
}

void Board::movePiece(Tile src, Tile dst) {
	piece(dst) = piece(src);
	piece(src) = Piece::NONE;
}

bool Board::isInBound(Tile tile) const {
	if (tile[0] < 0)
		return false;
	if (tile[0] >= _width)
		return false;
	if (tile[1] < 0)
		return false;
	if (tile[1] >= _height)
		return false;
	return true;
}

// ================================================================

std::shared_ptr<Board> Board::factoryStandard() {

	static const Piece K = Piece{PLAYER_WHITE, TYPE_KING};
	static const Piece Q = Piece{PLAYER_WHITE, TYPE_QUEEN};
	static const Piece R = Piece{PLAYER_WHITE, TYPE_ROOK};
	static const Piece B = Piece{PLAYER_WHITE, TYPE_BISHOP};
	static const Piece N = Piece{PLAYER_WHITE, TYPE_KNIGHT};
	static const Piece P = Piece{PLAYER_WHITE, TYPE_PAWN};

	static const Piece k = Piece{PLAYER_BLACK, TYPE_KING};
	static const Piece q = Piece{PLAYER_BLACK, TYPE_QUEEN};
	static const Piece r = Piece{PLAYER_BLACK, TYPE_ROOK};
	static const Piece b = Piece{PLAYER_BLACK, TYPE_BISHOP};
	static const Piece n = Piece{PLAYER_BLACK, TYPE_KNIGHT};
	static const Piece p = Piece{PLAYER_BLACK, TYPE_PAWN};

	static const Piece _ = Piece::NONE;

	Piece pieces[BOARD_HEIGHT_DEFAULT][BOARD_WIDTH_DEFAULT] =
		   /* ******************************** */
		   /*   *     A B C D E F G H      *   */
		{  /* 1 */  { R,N,B,Q,K,B,N,R },  /* 0 */
		   /* 2 */  { P,P,P,P,P,P,P,P },  /* 1 */
		   /* 3 */  { _,_,_,_,_,_,_,_ },  /* 2 */
		   /* 4 */  { _,_,_,_,_,_,_,_ },  /* 3 */
		   /* 5 */  { _,_,_,_,_,_,_,_ },  /* 4 */
		   /* 6 */  { _,_,_,_,_,_,_,_ },  /* 5 */
		   /* 7 */  { p,p,p,p,p,p,p,p },  /* 6 */
		   /* 8 */  { r,n,b,q,k,b,n,r }   /* 7 */  };
		   /*   *     0 1 2 3 4 5 6 7      *   */
		   /* ******************************** */
	return make_shared<Board>(pieces);
}

std::shared_ptr<Board> Board::factoryEmpty() {
	static const Piece _ = Piece::NONE;
	Piece pieces[BOARD_HEIGHT_DEFAULT][BOARD_WIDTH_DEFAULT] =
		   /* ******************************** */
		   /*   *     A B C D E F G H      *   */
		{  /* 1 */  { _,_,_,_,_,_,_,_ },  /* 0 */
		   /* 2 */  { _,_,_,_,_,_,_,_ },  /* 1 */
		   /* 3 */  { _,_,_,_,_,_,_,_ },  /* 2 */
		   /* 4 */  { _,_,_,_,_,_,_,_ },  /* 3 */
		   /* 5 */  { _,_,_,_,_,_,_,_ },  /* 4 */
		   /* 6 */  { _,_,_,_,_,_,_,_ },  /* 5 */
		   /* 7 */  { _,_,_,_,_,_,_,_ },  /* 6 */
		   /* 8 */  { _,_,_,_,_,_,_,_ }   /* 7 */  };
		   /*   *     0 1 2 3 4 5 6 7      *   */
		   /* ******************************** */
	return make_shared<Board>(pieces);
}

const Tile Board::INVALID_TILE = Tile(-1, -1);
