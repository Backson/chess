#include "View.hpp"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include "Rules.hpp"

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
	_orientation(orientation),
	_border_size(border_size),
	_decoration(border_size > 0.0),
	_x(0.0), _y(0.0),

	_position(board_width, board_height),
	_last_orientation(WHITE_AT_THE_BOTTOM),
	_selection(-1, -1),
	_cursor(-1, -1),

	_buffer(nullptr)
{
	_buffer = al_create_bitmap(getPanelWidthPixels(), getPanelHeightPixels());
	al_set_target_bitmap(_buffer);
	drawPanel(0.0, 0.0, _position, _selection);
}

View::~View() {
	al_destroy_bitmap(_buffer);
}

void View::draw(float x, float y, const Position &position, Tile selection, Type promoSelection) {
	ALLEGRO_BITMAP *target = al_get_target_bitmap();
	updateBuffer(position, selection, promoSelection);
	al_set_target_bitmap(target);
	al_draw_bitmap(_buffer, x, y, 0);
}

void View::updateBuffer(const Position &position, Tile selection, Type promoSelection) {
	al_set_target_bitmap(_buffer);

	ALLEGRO_MOUSE_STATE mouse;
	al_get_mouse_state(&mouse);
	Tile cursor = getTileAt(mouse.x, mouse.y);

	int counter = 0;

	for (int8 yy = 0; yy < _board_height; ++yy)
		for (int8 xx = 0; xx < _board_width; ++xx)
		{
			Tile algebraic = Tile(xx, yy);

			bool redraw = false;
			if (_selection == algebraic && selection != algebraic)
				redraw = true;
			else if (_selection != algebraic && selection == algebraic)
				redraw = true;
			else if (_cursor == algebraic && cursor != algebraic)
				redraw = true;
			else if (_cursor != algebraic && cursor == algebraic)
				redraw = true;
			else if (_position[algebraic] != position[algebraic])
				redraw = true;
			else if(_last_orientation != _orientation)
				redraw = true;

			if (!redraw)
				continue;

			++counter;

			Tile displayed = convertAlgebraicToDisplayed(algebraic);
			float piece_x = _border_size + displayed[0] * getTileSizePixels();
			float piece_y = _border_size + displayed[1] * getTileSizePixels();
			const Piece& piece = position[algebraic];
			TileColor tile_color = (xx + yy) % 2 == 0 ? TILE_DARK : TILE_LIGHT;

			drawPiece(piece_x, piece_y, piece, tile_color);

			if (algebraic == selection) {
				drawSelection(piece_x, piece_y);
			}
			if (algebraic == cursor) {
				bool has_sel = position.isInBound(selection);
				bool owns_piece = position[cursor].player ==  position.active_player();
				if(has_sel || owns_piece)
					drawCursor(piece_x, piece_y);
			}
		}

	_last_orientation = _orientation;
	_position = position;
	_selection = selection;
	_cursor = cursor;

	// TODO draw promotion selector and buttons
}

void View::drawPanel(float x, float y, const Position &position, Tile selection) {
	_x = x;
	_y = y;
	_position = position;

	if (_border_size > 0.0) {
		drawBorder(x, y);
		if (_decoration) {
			drawBorderDecoration(x, y);
		}
	}
	const float border = getBorderSizePixels();
	drawBoard(x + border, y + border, position, selection);
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

void View::drawBoard(float x, float y, const Position &position, Tile selection) {
	ALLEGRO_MOUSE_STATE mouse;
	al_get_mouse_state(&mouse);
	Tile cursor = getTileAt(mouse.x, mouse.y);

	for (int8 yy = 0; yy < position.height(); ++yy)
		for (int8 xx = 0; xx < position.width(); ++xx)
		{
			Tile algebraic = Tile(xx, yy);
			Tile displayed = convertAlgebraicToDisplayed(algebraic);
			float piece_x = x + displayed[0] * getTileSizePixels();
			float piece_y = y + displayed[1] * getTileSizePixels();
			const Piece& piece = position[algebraic];
			TileColor tile_color = (xx + yy) % 2 == 0 ? TILE_DARK : TILE_LIGHT;
			drawPiece(piece_x, piece_y, piece, tile_color);

			if (algebraic == selection) {
				drawSelection(piece_x, piece_y);
			}
			if (algebraic == cursor) {
				bool has_sel = position.isInBound(selection);
				bool owns_piece = position[cursor].player ==  position.active_player();
				if(has_sel || owns_piece)
					drawCursor(piece_x, piece_y);
			}
		}
}

void View::drawPiece(float x, float y, const Piece& piece, int tile_color) {
	Player player = piece.type == TYPE_NONE ? PLAYER_WHITE : piece.player;
	ALLEGRO_BITMAP* image = _image_tile[piece.type + 1][player][tile_color];
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

void View::drawPromotionSelector(float x, float y) {
	// TODO implement
}

void View::drawButtons() {
	// TODO implement
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

float View::getBoardHeightPixels() const {
	bool sideway = isSideway(_orientation);
	auto height = sideway ? getBoardWidth() : getBoardHeight();
	return height * getTileSizePixels();
}

int View::getBoardWidth() const {
	return _board_width;
}

int View::getBoardHeight() const {
	return _board_height;
}

Orientation View::getOrientation() const {
	return _orientation;
}

void View::setOrientation(Orientation orientation) {
	_orientation = orientation;
}

Tile View::convertAlgebraicToDisplayed(Tile algebraic) const {
	switch (_orientation)
	{
		case WHITE_AT_THE_BOTTOM: {
			int8 y = _board_height - algebraic[1] - 1;
			int8 x = algebraic[0];
			return Tile(x, y);
		}
		case WHITE_ON_TOP: {
			int8 y = algebraic[1];
			int8 x = _board_width - algebraic[0] - 1;
			return Tile(x, y);
		}
		case WHITE_ON_THE_LEFT: {
			int8 y = algebraic[0];
			int8 x = algebraic[1];
			return Tile(x, y);
		}
		case WHITE_ON_THE_RIGHT: {
			int8 y = _board_width - algebraic[0] - 1;
			int8 x = _board_height - algebraic[1] - 1;
			return Tile(x, y);
		}
		default:
			assert (false);
	} // switch (rotation)
}

Tile View::convertDisplayedToAlgebraic(Tile displayed) const {
	switch (_orientation)
	{
		case WHITE_AT_THE_BOTTOM: {
			int8 y = _board_height - displayed[1] - 1;
			int8 x = displayed[0];
			return Tile(x, y);
		}
		case WHITE_ON_TOP: {
			int8 y = displayed[1];
			int8 x = _board_width - displayed[0] - 1;
			return Tile(x, y);
		}
		case WHITE_ON_THE_LEFT: {
			int8 y = displayed[0];
			int8 x = displayed[1];
			return Tile(x, y);
		}
		case WHITE_ON_THE_RIGHT: {
			/* this is NOT copy&paste from the function above.  Here the
			 * board width and height are swapped.  This is because the
			 * coordinate transformation is a rotation and a translation
			 * and both operations have to switch orders when tranforming back.
			 */
			int8 y = _board_height - displayed[0] - 1;
			int8 x = _board_width - displayed[1] - 1;
			return Tile(x, y);
		}
		default:
			assert (false);
	} // switch (rotation)
}

Tile View::getTileAt(float x, float y) {
	float dx = x - getBorderSizePixels() - _x;
	float dy = y - getBorderSizePixels() - _y;

	if (dx >= getBoardWidthPixels() || dx < 0
		|| dy >= getBoardHeightPixels() || dy < 0)
	{
		return Tile((int8)-1, (int8)-1);
	}

	auto tile_size = getTileSizePixels();
	Tile displayed = Tile((int8)(dx / tile_size), (int8)(dy / tile_size));
	Tile algebraic = convertDisplayedToAlgebraic(displayed);
	return algebraic;
}

Type View::getPromotionTypeAt(float x, float y) {
	// TODO implement
	return TYPE_QUEEN;
}

int View::getButtonAt(float x, float y) {
	// TODO implement
	return BUTTON_NONE;
}

int View::initialize() {
	ALLEGRO_BITMAP* tile_map = al_load_bitmap("tiles_std.tga");

	if (!tile_map) {
		return -1;
	}

	const float tile_size = _tile_size;

	// activate alpha blending
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

	for (int type = -1; type < 6; ++type)
	for (int player = 0; player < 2; ++player)
	for (int tile = 0; tile < 2; ++tile)
	{
		// background
		ALLEGRO_BITMAP** i = &_image_tile[type + 1][player][tile];
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
					tile_size * (type + 1), /* source image coord */
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
