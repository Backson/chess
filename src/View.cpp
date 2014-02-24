/** @file View.cpp
 *
 */

#include "View.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include "GameModel.h"
#include "Board.h"
#include "constants.h"
#include "Move.h"
#include "Piece.h"
#include "Rules.h"

bool isSideway(Orientation orientation) {
	switch (orientation) {
		case WHITE_ON_TOP:
		case WHITE_AT_THE_BOTTOM:
			return false;
		case WHITE_ON_THE_LEFT:
		case WHITE_ON_THE_RIGHT:
			return true;
		default:
			assert (false);
	}
}

View::View(int board_width, int board_height, Orientation orientation, float border_size) :
	_board_width(board_width), _board_height(board_height),
	_orientation(orientation), _border_size(border_size)
{
	_decoration = border_size > 0.0;
	_x = 0.0;
	_y = 0.0;
	_board = Board(board_width, board_height);
	_selection = Square(-1, -1);
	_cursor = Square(-1, -1);
	_buffer = al_create_bitmap(getPanelWidthPixels(), getPanelHeightPixels());
}

View::~View() {
    al_destroy_bitmap(_buffer);
}

void View::draw(float x, float y, const GameModel &model, Square selection) {
	drawPanel(x, y, model.getBoard(), selection);
}

void View::drawPanel(float x, float y, const Board &board, Square selection) {
	_x = x;
	_y = y;
	_board = board;
	
	if (_border_size > 0.0) {
		drawBorder(x, y);
		if (_decoration) {
			drawBorderDecoration(x, y);
		}
	}
	const float border = getBorderSizePixels();
	drawBoard(x + border, y + border, board, selection);
}

void View::drawBorder(float x, float y) {
	auto color = al_map_rgb(0, 0, 0);
	float border = getBorderSizePixels();
	float width = getPanelWidthPixels();
	float height = getPanelHeightPixels();
	
	// top
    al_draw_filled_rectangle(
		x, y, x + width, y + border, color);
	// bottom
    al_draw_filled_rectangle(
		x, y + height - border, x + width, y + height, color);
	// left
    al_draw_filled_rectangle(
		x, y + border, x + border, y + border + height, color);
	// right
    al_draw_filled_rectangle(
		x + width - border, y + border, x + width, y + border + height, color);
}

void View::drawBorderDecoration(float x, float y) {
	// TODO implement
}

void View::drawBoard(float x, float y, const Board& board, Square selection) {
	ALLEGRO_MOUSE_STATE mouse;
	al_get_mouse_state(&mouse);
	Square cursor = getSquareAt(mouse.x, mouse.y);
	
    for (int rank = 0; rank < board.getHeight(); ++rank)
	for (int file = 0; file < board.getWidth(); ++file)
	{
		Square algebraic = Square(rank, file);
		Square displayed = convertAlgebraicToDisplayed(algebraic);
		float piece_x = x + displayed.getFile() * getTileSizePixels();
		float piece_y = y + displayed.getRank() * getTileSizePixels();
		const Piece& piece = board.getPiece(algebraic);
		Tile whichTile = (rank + file) % 2 == 0 ? TILE_DARK : TILE_LIGHT;
		drawPiece(piece_x, piece_y, piece, whichTile);
		
		if (algebraic == selection) {
			drawSelection(piece_x, piece_y);
		}
		if (algebraic == cursor) {
			if (board.getPiece(cursor).getType() != TYPE_NONE)
				drawCursor(piece_x, piece_y);
		}
	}
}

void View::drawPiece(float x, float y, const Piece& piece, int whichTile) {
    int player = piece.getPlayer() == PLAYER_WHITE ? 0 : 1;
	Type type = piece.getType();
    ALLEGRO_BITMAP* image = _image_tile[type][player][whichTile];

    al_draw_bitmap(image, x, y, 0);
}

void View::drawSelection(float x, float y) {
	auto color = al_map_rgb(0, 255, 0);
	auto l = getTileSizePixels();
	auto d = 4.0;
	auto x1 = x + d * 0.5;
	auto y1 = y + d * 0.5;
	auto x2 = x + l - d * 0.5;
	auto y2 = y + l - d * 0.5;
	al_draw_rectangle(x1, y1, x2, y2, color, d);
}

void View::drawCursor(float x, float y) {
	auto color = al_map_rgb(255, 0, 0);
	auto l = getTileSizePixels();
	auto d = 1.0;
	auto x1 = x + d * 0.5;
	auto y1 = y + d * 0.5;
	auto x2 = x + l - d * 0.5;
	auto y2 = y + l - d * 0.5;
	al_draw_rectangle(x1, y1, x2, y2, color, d);
}

float View::getBorderSizePixels() const {
	return _border_size;
}

float View::getTileSizePixels() const {
	return _tile_size;
}

float View::getPanelWidthPixels() const {
	return getBoardWidthPixels() + 2 * getBorderSizePixels();
}

float View::getPanelHeightPixels() const {
	return getBoardHeightPixels() + 2 * getBorderSizePixels();
}

float View::getBoardWidthPixels() const {
	bool sideway = isSideway(_orientation);
	auto width = sideway ? getBoardHeight() : getBoardWidth();
	return width * getTileSizePixels();
}

int View::getBoardWidth() const {
	return _board_width;
}

int View::getBoardHeight() const {
	return _board_height;
}

float View::getBoardHeightPixels() const {
	bool sideway = isSideway(_orientation);
	auto height = sideway ? getBoardWidth() : getBoardHeight();
	return height * getTileSizePixels();
}

Square View::convertAlgebraicToDisplayed(Square algebraic) const {
    switch (_orientation)
    {
		case WHITE_AT_THE_BOTTOM: {
			Coord rank = BOARD_HEIGHT - algebraic.getRank() - 1;
			Coord file = algebraic.getFile();
			return Square(rank, file);
		}
		case WHITE_ON_TOP: {
			Coord rank = algebraic.getRank();
			Coord file = BOARD_WIDTH - algebraic.getFile() - 1;
			return Square(rank, file);
		}
		case WHITE_ON_THE_LEFT: {
			Coord rank = algebraic.getFile();
			Coord file = algebraic.getRank();
			return Square(rank, file);
		}
		case WHITE_ON_THE_RIGHT: {
			Coord rank = BOARD_WIDTH - algebraic.getFile() - 1;
			Coord file = BOARD_HEIGHT - algebraic.getRank() - 1;
			return Square(rank, file);
		}
		default:
			assert (false);
    } // switch (rotation)
}

Square View::convertDisplayedToAlgebraic(Square displayed) const {
    switch (_orientation)
    {
		case WHITE_AT_THE_BOTTOM: {
			Coord rank = BOARD_HEIGHT - displayed.getRank() - 1;
			Coord file = displayed.getFile();
			return Square(rank, file);
		}
		case WHITE_ON_TOP: {
			Coord rank = displayed.getRank();
			Coord file = BOARD_WIDTH - displayed.getFile() - 1;
			return Square(rank, file);
		}
		case WHITE_ON_THE_LEFT: {
			Coord rank = displayed.getFile();
			Coord file = displayed.getRank();
			return Square(rank, file);
		}
		case WHITE_ON_THE_RIGHT: {
			/* this is NOT copy&paste from the function above.  Here the
			 * board width and height are swapped.  This is because the
			 * coordinate transformation is a rotation and a translation
			 * and both operations have to switch orders when tranforming back.
			 */
			Coord rank = BOARD_HEIGHT - displayed.getFile() - 1;
			Coord file = BOARD_WIDTH - displayed.getRank() - 1;
			return Square(rank, file);
		}
		default:
			assert (false);
    } // switch (rotation)
}

Square View::getSquareAt(float x, float y) {
    float dx = x - getBorderSizePixels() - _x;
    float dy = y - getBorderSizePixels() - _y;
	
    if (dx >= getBoardWidthPixels() || dx < 0
		|| dy >= getBoardHeightPixels() || dy < 0)
    {
        return Square(-1, -1);
    }
	
	auto tile_size = getTileSizePixels();
	Square displayed = Square(dy / tile_size, dx / tile_size);
	Square algebraic = convertDisplayedToAlgebraic(displayed);
	return algebraic;
}

int View::initialize() {
    ALLEGRO_BITMAP* tile_map = al_load_bitmap("tiles_std.tga");
	
	if (!tile_map) {
		return -1;
	}
	
	const float tile_size = _tile_size;
	
	// activate alpha blending
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    for (int type = 0; type < 7; ++type)
	for (int player = 0; player < 2; ++player)
	for (int tile = 0; tile < 2; ++tile)
	{
		// background
		ALLEGRO_BITMAP** i = &_image_tile[type][player][tile];
		*i = al_create_bitmap(_tile_size, _tile_size);
		al_set_target_bitmap(*i);
		al_draw_bitmap_region(
				tile_map, /* source image */
				0.0, tile_size * tile, /* source image coord */
				tile_size, tile_size, /* source image size */
				0.0, 0.0, /* destination image coord */
				0 /* flags */
		);
		// piece
		if (type != TYPE_NONE)
			al_draw_bitmap_region(
					tile_map, /* source image */
					tile_size * type, /* source image coord */
					tile_size * player,
					tile_size, tile_size, /* source image size */
					0.0, 0.0, /* destination image coord */
					0 /* flags */
			);
    }

    al_destroy_bitmap(tile_map);
	_is_initialized = true;
    return 0;
}

void View::deinitialize() {
	if (_is_initialized == false)
		return;
	
    for (int type = 0; type < 7; ++type)
	for (int player = 0; player < 2; ++player)
	for (int tile = 0; tile < 2; ++tile)
	{
		al_destroy_bitmap(_image_tile[type][player][tile]);
		_image_tile[type][player][tile] = 0;
	}
	_is_initialized = false;
}

const float View::_tile_size = TILE_SIZE;
bool View::_is_initialized = false;
ALLEGRO_BITMAP* View::_image_tile[7][2][2];
