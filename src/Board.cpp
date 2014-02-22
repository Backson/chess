/** @file Board.cpp
 *
 */

#include "Board.h"

#include "constants.h"
#include "Piece.h"
#include "Square.h"

Board::Board() :
	_width(BOARD_WIDTH), _height(BOARD_HEIGHT)
{
    this->_pieces = new Piece*[_height];
    for (Coord rank = 0; rank < _height; ++rank)
    {
    	this->_pieces[rank] = new Piece[_width];

        for (Coord file = 0; file < _width; ++file)
        {
            this->_pieces[rank][file] = Piece::NONE;
        }
    }
}

Board::Board(int width, int height) :
	_width(width), _height(height)
{
    this->_pieces = new Piece*[_height];
    for (Coord rank = 0; rank < _height; ++rank)
    {
    	this->_pieces[rank] = new Piece[_width];

        for (Coord file = 0; file < _width; ++file)
        {
            this->_pieces[rank][file] = Piece::NONE;
        }
    }
}

Board::Board(const Board& other) :
	_width(other._width), _height(other._height)
{
    this->_pieces = new Piece*[_height];
    for (Coord rank = 0; rank < _height; ++rank)
    {
    	this->_pieces[rank] = new Piece[_width];

        for (Coord file = 0; file < _width; ++file)
        {
            this->_pieces[rank][file] = other._pieces[rank][file];
        }
    }
}

Board::Board(const Piece pieces[BOARD_HEIGHT][BOARD_WIDTH]) :
	_width(BOARD_WIDTH), _height(BOARD_HEIGHT)
{
    this->_pieces = new Piece*[_height];
    for (Coord rank = 0; rank < _height; ++rank)
    {
    	this->_pieces[rank] = new Piece[_width];

        for (Coord file = 0; file < _width; ++file)
        {
            this->_pieces[rank][file] = pieces[rank][file];
        }
    }
}

Board::Board(const Piece **pieces) :
	_width(BOARD_WIDTH), _height(BOARD_HEIGHT)
{
    this->_pieces = new Piece*[_height];
    for (Coord rank = 0; rank < _height; ++rank)
    {
    	this->_pieces[rank] = new Piece[_width];

        for (Coord file = 0; file < _width; ++file)
        {
            this->_pieces[rank][file] = pieces[rank][file];
        }
    }
}

Board::Board(const Piece **pieces, int width, int height) :
	_width(width), _height(height)
{
    this->_pieces = new Piece*[_height];
    for (Coord rank = 0; rank < _height; ++rank)
    {
    	this->_pieces[rank] = new Piece[_width];

        for (Coord file = 0; file < _width; ++file)
        {
            this->_pieces[rank][file] = pieces[rank][file];
        }
    }
}

Board::~Board()
{
    for (Coord rank = 0; rank < _height; ++rank)
    {
    	delete[] _pieces[rank];
    }
    delete[] _pieces;
}

Board& Board::operator=(const Board& rhs)
{
	if (this == &rhs) return *this; // handle self assignment
	
	auto width = rhs._width;
	auto height = rhs._height;
	
	bool resize = _width != width || _height != height;
	if (resize) {
		for (Coord rank = 0; rank < height; ++rank) {
			delete[] _pieces[rank];
		}
		delete[] _pieces;
		_width = width;
		_height = height;
		this->_pieces = new Piece*[height];
	}
	
    for (Coord rank = 0; rank < height; ++rank)
    {
		if (resize) this->_pieces[rank] = new Piece[width];
		
        for (Coord file = 0; file < width; ++file)
        {
            this->_pieces[rank][file] = rhs._pieces[rank][file];
        }
    }
    return *this;
}

bool Board::operator==(const Board& other) const
{
    // handle self-comparison
    if (this == &other) return true;

    // handle boards of different sizes
    if (this->getHeight() != other.getHeight()) return false;
    if (this->getWidth() != other.getWidth()) return false;

    // compare per piece
    for (Coord rank = 0; rank < getHeight(); ++rank)
    {
        for (Coord file = 0; file < getWidth(); ++file)
        {
            if (this->_pieces[rank][file] != other._pieces[rank][file])
                return false;
        }
    }

    return true;
}
bool Board::operator!=(const Board& other) const
{
    return !operator==(other);
}

Piece Board::getPiece(Coord rank, Coord file) const
{
    if (!isInBound(rank, file))
        return Piece::NONE;
    return _pieces[rank][file];
}

Piece Board::getPiece(Square square) const
{
    // delegate
    return getPiece(square.getRank(), square.getFile());
}

void Board::setPiece(Coord rank, Coord file, Piece piece)
{
    if (!isInBound(rank, file)) return;
    _pieces[rank][file] = piece;
}
void Board::setPiece(Square square, Piece piece)
{
    // delegate
    setPiece(square.getRank(), square.getFile(), piece);
}

void Board::removePiece(Coord rank, Coord file)
{
    _pieces[rank][file] = 0;
}
void Board::removePiece(Square square)
{
    removePiece(square.getRank(), square.getFile());
}

void Board::movePiece(Coord rank_src, Coord file_src, Coord rank_dest, Coord file_dest)
{
    _pieces[rank_dest][file_dest] = _pieces[rank_src][file_src];
    _pieces[rank_src][file_src] = Piece::NONE;
}
void Board::movePiece(Square src, Square dest)
{
    movePiece(src.getRank(), src.getFile(), dest.getRank(), dest.getFile());
}

Coord Board::getWidth() const
{
    return _width;
}
Coord Board::getHeight() const
{
    return _height;
}
bool Board::isInBound(Coord rank, Coord file) const
{
    return  file >= 0 && file < getWidth()
         && rank >= 0 && rank < getHeight();
}
bool Board::isInBound(Square square) const
{
    return  isInBound(square.getRank(), square.getFile());
}

// ================================================================

Piece Board::getFirstOnPath(        Coord rank_src, Coord file_src,
                                    Coord rank_dest, Coord file_dest,
                                    Coord rank_step, Coord file_step) const
{
    if (!isInBound(rank_src, file_src))
        throw -1;
    if (!isInBound(rank_dest, file_dest))
        throw -2;
    if (!isStepSizeLegal(rank_src, file_src, rank_dest, file_dest, rank_step, file_step))
        throw -3;

    // initialize
    Coord rank = rank_src - rank_step;
    Coord file = file_src - file_step;
    do
    {
        // iterate
        rank += rank_step;
        file += file_step;

        // check
        Piece p = _pieces[rank][file];
        if (p.getType() != TYPE_NONE)
        {
            return p;
        }
    }
    while (rank != rank_dest && file != file_dest);

    // no were found
    return Piece();
}

Piece Board::getFirstBetween(       Coord rank_src, Coord file_src,
                                    Coord rank_dest, Coord file_dest,
                                    Coord rank_step, Coord file_step) const
{
    if (!isInBound(rank_src, file_src))
        throw -1;
    if (!isInBound(rank_dest, file_dest))
        throw -2;
    if (!isStepSizeLegal(rank_step, file_step))
        throw -3;

    Coord rank_d = rank_dest - rank_src;
    Coord file_d = file_dest - file_src;
    if (rank_d != 0 && file_d != 0 && rank_d * rank_d != file_d * file_d)
        throw -4;

    // catch neighboored Squares
    if (rank_d <= 1 && rank_d >= -1 && file_d <= 1 && file_d >= -1)
        return Piece();

    return getFirstOnPath(  rank_src + rank_step, file_src + file_step,
                            rank_dest - rank_step, file_dest - file_step,
                            rank_step, file_step);
}

Piece Board::getFirstInDirection(   Coord rank_src, Coord file_src,
                                    Coord rank_step, Coord file_step) const
{
    // check input
    if (!isInBound(rank_src, file_src))
        throw -1;
    if (!isStepSizeLegal(rank_step, file_step))
        throw -3;

    // calculate how many squares are in both directions, until board ends
    Coord rank_d;
    Coord file_d;
    switch (rank_step)
    {
        case 0:     rank_d = 0;                             break;
        case 1:     rank_d = getHeight() - rank_src - 1;    break;
        case -1:    rank_d = rank_src;                      break;
        default:    throw -5;                               break;
    }
    switch (file_step)
    {
        case 0:     file_d = 0;                             break;
        case 1:     file_d = getWidth() - file_src - 1;     break;
        case -1:    file_d = file_src;                      break;
        default:    throw -5;                               break;
    }

    //we are moving towards a wall or corner!
    if (rank_d == 0 && file_d == 0)
        return Piece();

    // locate the square, the piece will end up if going the whole path
    Coord rank_dest;
    Coord file_dest;
    if (rank_d == 0)
    {
        // piece is moving horizontally
        rank_dest = rank_src;
        file_dest = file_src + file_step * file_d;
    }
    else if (file_d == 0)
    {
        // piece is moving vertically
        rank_dest = rank_src + rank_step * rank_d;
        file_dest = file_src;
    }
    else
    {
        // piece is moving diagonally
        int minimum; //< min(abs(rank_d), abs(file_d))
        if (rank_d * rank_d < file_d * file_d)
            minimum = rank_d < 0 ? -rank_d : rank_d;
        else
            minimum = file_d < 0 ? -file_d : file_d;

        rank_dest = rank_src + rank_step * minimum;
        file_dest = file_src + file_step * minimum;
    }

    return getFirstOnPath(  rank_src + rank_step, file_src + file_step,
                            rank_dest, file_dest, rank_step, file_step);
}

Piece Board::getFirstOnPath(    Square src, Square dest,
                                Coord rank_step, Coord file_step) const
{
    // delegate
    return getFirstOnPath(src.getRank(), src.getFile(), dest.getRank(), dest.getFile(), rank_step, file_step);
}

Piece Board::getFirstBetween(   Square src, Square dest,
                                Coord rank_step, Coord file_step) const
{
    // delegate
    return getFirstBetween(src.getRank(), src.getFile(), dest.getRank(), dest.getFile(), rank_step, file_step);
}

Piece Board::getFirstInDirection(   Square src,
                                    Coord rank_step, Coord file_step) const
{
    // delegate
    return getFirstInDirection(src.getRank(), src.getFile(), rank_step, file_step);
}

Piece Board::getFirstOnPath(    Coord rank_src,  Coord file_src,
                                Coord rank_dest, Coord file_dest) const
{
    Coord rank_d = rank_dest - rank_src;
    Coord rank_step = rank_d / (rank_d < 0 ? rank_d : -rank_d);

    Coord file_d = file_dest - file_src;
    Coord file_step = file_d / (file_d < 0 ? file_d : -file_d);

    return getFirstOnPath(rank_src, file_src, rank_dest, file_dest, rank_step, file_step);
}
Piece Board::getFirstBetween(   Coord rank_src,  Coord file_src,
                                Coord rank_dest, Coord file_dest) const
{
    Coord rank_d = rank_dest - rank_src;
    Coord rank_step;
    if (rank_d == 0)
        rank_step = 0;
    else
        rank_step = rank_d / (rank_d > 0 ? rank_d : -rank_d);

    Coord file_d = file_dest - file_src;
    Coord file_step;
    if (file_d == 0)
        file_step = 0;
    else
        file_step = file_d / (file_d > 0 ? file_d : -file_d);

    return getFirstBetween(rank_src, file_src, rank_dest, file_dest, rank_step, file_step);
}

Piece Board::getFirstOnPath(Square src, Square dest) const
{
    // delegate
    return getFirstOnPath(   src.getRank(),  src.getFile(),
                            dest.getRank(), dest.getFile());
}
Piece Board::getFirstBetween(Square src, Square dest) const
{
    // delegate
    return getFirstBetween(  src.getRank(),  src.getFile(),
                            dest.getRank(), dest.getFile());
}

// ================================================================

Board Board::factoryStandard()
{
    DEFINE_PIECE_SHORTCUTS;
    Piece pieces[BOARD_HEIGHT][BOARD_WIDTH] =
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
    return Board(pieces);
}

Board Board::factoryEmpty()
{
    DEFINE_PIECE_SHORTCUTS;
    Piece pieces[BOARD_HEIGHT][BOARD_WIDTH] =
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
    return Board(pieces);
}

bool Board::isStepSizeLegal(Coord rank_step, Coord file_step)
{
    if (rank_step < -1 || 1 < rank_step)
        return false;

    if (file_step < -1 || 1 < file_step)
        return false;

    if (rank_step == 0 && file_step == 0)
        return false;

    return true;
}

bool Board::isStepSizeLegal(    Coord rank_src,  Coord file_src,
                                Coord rank_dest, Coord file_dest,
                                Coord rank_step, Coord file_step)
{
    if (!isStepSizeLegal(rank_step, file_step))
        return false;

    Coord rank_d = rank_dest - rank_src;
    Coord file_d = file_dest - file_src;
    if (rank_d * rank_d != file_d * file_d)
        return false;

    return true;
}
