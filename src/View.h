/** @file View.h
 *
 */

#ifndef View_h
#define View_h

#include <allegro5/allegro.h>

#include "GameModel.h"
#include "Board.h"
#include "Piece.h"
#include "Square.h"
#include "types.h"

#define TILE_SIZE 50;

enum Tile {
	TILE_LIGHT,
	TILE_DARK,
};

enum Orientation {
    WHITE_ON_TOP,
    WHITE_ON_THE_LEFT,
    WHITE_ON_THE_RIGHT,
    WHITE_AT_THE_BOTTOM,
};

bool isSideway(Orientation);

class View
{
public:
	View() = delete;
	View(const View &) = delete;
	View(int board_width, int board_height,
		Orientation orientation, float border_size);
    ~View();

    void draw(float x, float y, const GameModel &model, Square selection);

private:
    void drawPanel(float x, float y, const Board &board, Square selection);
    void drawBorder(float x, float y);
    void drawBorderDecoration(float x, float y);
    void drawBoard(float x, float y, const Board &board, Square selection);
    void drawPiece(float x, float y, const Piece &piece, int whichTile);
    void drawSelection(float x, float y);
	void drawCursor(float x, float y);
	
public:
	
	float getBorderSizePixels() const;
	float getTileSizePixels() const;
    float getPanelWidthPixels() const;
    float getPanelHeightPixels() const;
    float getBoardWidthPixels() const;
    float getBoardHeightPixels() const;
	
	int getBoardWidth() const;
	int getBoardHeight() const;

	Square convertAlgebraicToDisplayed(Square) const;
	Square convertDisplayedToAlgebraic(Square) const;

    Square getSquareAt(float x, float y);

    static int initialize();
    static void deinitialize();
	
private:
	/// size of the board in tiles
	const int _board_width, _board_height;
	/// in which orientation to draw
    const Orientation _orientation;
	/// size of the border in pixels
	const float _border_size;
	
	/// true, if numbers and letters should be drawn next to the board
	bool _decoration;
	
	/// where was the board las drawn? (top left corner, border inclusive)
	int _x, _y;
	/// The board that was last drawn
    Board _board;
	/// which selection was drawn last frame (invalid if none was drawn)
    Square _selection;
	/// where the cursor was drawn last frame (invalid if none was drawn)
    Square _cursor;
	/// draw all the frames here (dirty rect) and blit this to target
	ALLEGRO_BITMAP* _buffer;

	/// size of a single tile in pixels (only square tiles)
	static const float _tile_size;
	/// was initialized() called?
	static bool _is_initialized;
	/// the sprites used to draw the board
    static ALLEGRO_BITMAP* _image_tile[7][2][2]; // type, player, tile
};

#endif // View_h
