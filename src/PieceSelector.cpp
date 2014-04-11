#include "PieceSelector.hpp"

#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>

#include "cstdio"

#include "Board.hpp"

PieceSelector::~PieceSelector() {
	if (_texture) {
		al_destroy_bitmap(_texture);
		_texture = nullptr;
	}

	if (_buffer) {
		al_destroy_bitmap(_texture);
		_buffer = nullptr;
	}
}

PieceSelector::PieceSelector(float x, float y) :
	_x(x), _y(y)
{
	_texture = al_load_bitmap("tiles_std.tga");
	if (!_texture) {
		// throw resource not found
	} else {
		int h = al_get_bitmap_height(_texture);
		_tile_size = h / 2;
	}

	_buffer = al_create_bitmap(_tile_size * 2, _tile_size * 6);

	disableAll();
}


void PieceSelector::draw() {
	ALLEGRO_BITMAP *target = al_get_target_bitmap();
	updateBuffer();
	al_set_target_bitmap(target);
	al_draw_bitmap(_buffer, _x, _y, 0);
}

Tile PieceSelector::getTileAt(float x, float y) {
	float dx = x - _x;
	float dy = y - _y;

	if (dx >= _tile_size * 2 || dx < 0
		|| dy >= _tile_size * 7 || dy < 0)
	{
		return Tile(-1, -1);
	}

	return Tile(dx / _tile_size, dy / _tile_size);
}

void PieceSelector::enableAll() {
	for (int i = 0; i < 14; ++i) {
		_button_states[i] = ButtonState::NORMAL;
	}
	updateBuffer(true);
}

void PieceSelector::disableAll() {
	for (int i = 0; i < 14; ++i) {
		_button_states[i] = ButtonState::GRAY;
	}
	updateBuffer(true);
}

void PieceSelector::enable(Piece piece) {
	button_state(piece) = ButtonState::NORMAL;
	drawButton(piece);
}

void PieceSelector::disable(Piece piece) {
	button_state(piece) = ButtonState::GRAY;
	drawButton(piece);
}

bool PieceSelector::is_enabled(Piece piece) const {
	return button_state(piece) != ButtonState::GRAY;
}

void PieceSelector::updateBuffer(bool force) {
	al_set_target_bitmap(_buffer);

	ALLEGRO_MOUSE_STATE mouse;
	al_get_mouse_state(&mouse);

	Tile tile = getTileAt(mouse.x, mouse.y);

	for (int i = 0; i < 2; ++i)
	for (int k = 0; k < 7; ++k) {
		Player player = static_cast<Player>(i);
		Type type = static_cast<Type>(k);
		Piece piece{player, type};
		ButtonState &old_state = button_state(piece);

		ButtonState new_state;
		bool hover = tile == Tile(i, k);
		bool pressed = mouse.buttons & 1;
		if (old_state == ButtonState::GRAY) {
			new_state = ButtonState::GRAY;
		} else if (!hover && !pressed) {
			new_state = ButtonState::NORMAL;
		} else if (hover && !pressed) {
			new_state = ButtonState::HOVER;
		} else if (hover && pressed) {
			new_state = ButtonState::NORMAL;
		} else {
			new_state = ButtonState::NORMAL;
		}

		if (force || old_state != new_state) {
			old_state = new_state;
			drawButton(piece);
		}
	}
}

void PieceSelector::drawButton(Piece piece) {
	al_set_target_bitmap(_buffer);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

	ButtonState state = button_state(piece);

	float src_x = _tile_size * (piece.type + 1);
	float src_y = _tile_size * piece.player;

	float dst_x = _tile_size * piece.player;
	float dst_y = _tile_size * piece.type;

	al_draw_filled_rectangle(
		dst_x, dst_y,
		dst_x + _tile_size, dst_y + _tile_size,
		al_color_name("lightgray")
	);

	al_draw_bitmap_region(
		_texture,
		src_x, src_y,
		_tile_size, _tile_size,
		dst_x, dst_y,
		0 /* flags */
	);

	switch (state) {
		case ButtonState::NORMAL:
			break;
		case ButtonState::PRESSED_HOVER:
		case ButtonState::PRESSED:
			al_draw_rectangle(
				dst_x + 2, dst_y + 2,
				dst_x + _tile_size - 2, dst_y + _tile_size - 2,
				al_color_name("green"),
				4 /* thickness */
			);
			if (state != ButtonState::PRESSED_HOVER)
				break;
		case ButtonState::HOVER:
			al_draw_rectangle(
				dst_x + 0.5, dst_y + 0.5,
				dst_x + _tile_size - 0.5, dst_y + _tile_size - 0.5,
				al_color_name("red"),
				1 /* thickness */
			);
			break;
		case ButtonState::PRESSING:
		case ButtonState::RELEASING:
			break;
		case ButtonState::GRAY:
			al_draw_filled_rectangle(
				dst_x, dst_y,
				dst_x + _tile_size, dst_y + _tile_size,
				al_map_rgba(0x7F, 0x7F, 0x7F, 0xAF)
			);
			break;
	} // switch
}
