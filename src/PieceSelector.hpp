#ifndef PIECE_SELECTOR_HPP
#define PIECE_SELECTOR_HPP

#include "stdtypes.hpp"
#include "Piece.hpp"
#include "Board.hpp"
#include <allegro5/allegro.h>

class PieceSelector {
public:

	enum class ButtonState : uint8 {
		NORMAL,
		HOVER,
		PRESSED,
		PRESSED_HOVER,
		PRESSING,
		RELEASING,
		GRAY,
	};

	~PieceSelector();
	PieceSelector() = delete;
	PieceSelector(float x, float y);
	PieceSelector(const PieceSelector &) = delete;
	PieceSelector &operator = (const PieceSelector &) = delete;

	void draw();
	Tile getTileAt(float x, float y);

	void enableAll();
	void disableAll();

	void enable(Piece);
	void disable(Piece);

	bool is_enabled(Piece) const;

	ButtonState button_state(Piece p) const {
		return _button_states[p.type * 2 + p.player];
	}

private:

	ButtonState &button_state(Piece p) {
		return _button_states[p.type * 2 + p.player];
	}

	void updateBuffer(bool force = false);
	void drawButton(Piece);

	/// where is this widget? top left corner, in pixels from window edge
	float _x, _y;

	/// draw all the frames here (dirty rect) and blit this to target
	ALLEGRO_BITMAP *_buffer = nullptr;

	/// last known state of the buttons (as drawn in the buffer)
	ButtonState _button_states[14];

	/// holds bare images of all the pieces
	ALLEGRO_BITMAP *_texture = nullptr;
	float _tile_size;
};


#endif // PIECE_SELECTOR_HPP
