/** @file Board.h
 *
 */

#ifndef Board_h
#define Board_h

#include "types.h"
#include "constants.h"
class Piece;
class Square;

class Board
{
public:
    Board();
	Board(int width, int height);
    Board(const Board& other);
    Board(const Piece[BOARD_HEIGHT][BOARD_WIDTH]);
    Board(const Piece**);
    Board(const Piece**, int width, int height);
    Board& operator=(const Board& other);
    ~Board();

    bool operator==(const Board& other) const;
    bool operator!=(const Board& other) const;
	
    Piece getPiece(Coord rank, Coord file) const;
    Piece getPiece(Square square) const;
    void setPiece(Coord rank, Coord file, Piece piece);
    void setPiece(Square square, Piece piece);
    //inline const Piece** getPieces() const {return pieces;};
    void removePiece(Coord rank, Coord file);
    void removePiece(Square square);
    void movePiece(Coord rank_src, Coord file_src, Coord rank_dest, Coord file_dest);
    void movePiece(Square src, Square dest);

    Coord getWidth() const;
    Coord getHeight() const;
    bool isInBound(Coord rank, Coord file) const;
    bool isInBound(Square square) const;

    static Board factoryStandard();
    static Board factoryEmpty();
private:
	int _width, _height;
    Piece** _pieces;

    static bool isStepSizeLegal(Coord rank_step, Coord file_step);
    static bool isStepSizeLegal(Coord rank_src,  Coord file_src,
                                Coord rank_dest, Coord file_dest,
                                Coord rank_step, Coord file_step);
};

#endif // Board_h
