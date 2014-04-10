#include <cstdio>
#include <memory>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "View.hpp"
#include "Game.hpp"
#include "Action.hpp"
#include "Piece.hpp"
#include "Rules.hpp"
#include "SpeedyBot.hpp"
#include "RandomBot.hpp"

using std::shared_ptr;
using std::move;

static const int TIMER_BPS = 1000;
static const int64 US_PER_TICK = 1e6 / TIMER_BPS;

class Main {
public:
	Main() = default;
	~Main();

	int run();

private:

	void startSystems();
	void createWindow();

	void loop();
	void updateFps();
	void handleEvents();
	void handleKeyEvent(int key);
	void handleClickEvent(int x, int y);

	void makeMove(Tile src, Tile dst);

	void drawFrame();

	ALLEGRO_DISPLAY *_display = nullptr;
	ALLEGRO_EVENT_QUEUE *_event_queue = nullptr;
	ALLEGRO_TIMER *_timer = nullptr;
	ALLEGRO_FONT *_font = nullptr;

	Game *_game = nullptr;
	Game::HistoryConstIter _iter;
	View *_view = nullptr;

	Bot *_bot = nullptr;

	bool _shutdown = false;
	int _fps_counter = 0;
	int _fps = 0;
	int64 _us_counter = 0;

	Tile _selection = Tile(-1, -1);
	Type _promo_selection = TYPE_QUEEN;

	bool _panic = nullptr;
};

int main(int argc, char **argv) {
	Main main;
	return main.run();
}

Main::~Main() {
	if (_bot) {
		delete _bot;
		_bot = nullptr;
	}
	if (_font) {
		al_destroy_font(_font);
		_font = nullptr;
	}
	if (_view) {
		delete _view;
		_view = nullptr;
	}
	if (_game) {
		delete _game;
		_game = nullptr;
	}
	View::deinitialize();
	if (_timer) {
		al_destroy_timer(_timer);
		_timer = nullptr;
	}
	if (_display) {
		al_destroy_display(_display);
		_display = nullptr;
	}
	if (_event_queue) {
		al_destroy_event_queue(_event_queue);
		_event_queue = nullptr;
	}
}

int Main::run() {
	startSystems();
	createWindow();

	if (_panic) {
		fprintf(stderr, "Initialization failed.  Terminating program...\n");
		return -1;
	}

	_bot = new SpeedyBot();
	_bot->reset(_game->current_situation());

	al_start_timer(_timer);
	loop();

	// exit program success
	return 0;
}

void Main::startSystems() {
	// init allegro
	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		_panic = true;
		return;
	}
	if (!al_install_keyboard()) {
		fprintf(stderr, "failed to initialize keyboard driver!\n");
		_panic = true;
		return;
	}
	if (!al_install_mouse()) {
		fprintf(stderr, "failed to initialize mouse driver!\n");
		_panic = true;
		return;
	}
	if (!al_init_image_addon()) {
		fprintf(stderr, "failed to initialize image loading addon!\n");
		_panic = true;
		return;
	}
	if (!al_init_primitives_addon()) {
		fprintf(stderr, "failed to initialize primitives drawing addon!\n");
		_panic = true;
		return;
	}
	/*if (!al_init_font_addon()) {
		fprintf(stderr, "failed to initialize font addon!\n");
		_panic = true;
		return;
	}*/
	al_init_font_addon(); // returns bool in Allegro 5.1
	if (!al_init_ttf_addon()) {
		fprintf(stderr, "failed to initialize ttf addon!\n");
		_panic = true;
		return;
	}
}

void Main::createWindow() {
	_font = al_load_font("Bevan.ttf", -32, ALLEGRO_TTF_MONOCHROME);
	if (!_font) {
		_panic = true;
		return;
	}

	_timer = al_create_timer(ALLEGRO_BPS_TO_SECS(TIMER_BPS));
	if (!_timer) {
		_panic = true;
		return;
	}

	if (View::initialize()) {
		fprintf(stderr, "failed to initialize view!\n");
		_panic = true;
		return;
	}

	// init game model
	{
		shared_ptr<Board> shared_board = Board::factoryStandard();
		Situation situation(move(*shared_board), PLAYER_WHITE);
		shared_board.reset();
		_game = new Game(situation);
		_iter = --_game->history().end();

		const Situation &current = _game->current_situation();
		auto w = current.width();
		auto h = current.height();
		_view = new View(w, h, WHITE_AT_THE_BOTTOM, 20);
	}

	// init display
	{
		al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_OPENGL);
		int w = _view->getPanelWidthPixels();
		int h = _view->getPanelHeightPixels();
		_display = al_create_display(w + 200, h + 50);
		if (!_display) {
			fprintf(stderr, "failed to create display!\n");
			_panic = true;
			return;
		}
	}

	// show system mouse cursor
	if (!al_show_mouse_cursor(_display)) {
		fprintf(stderr, "WARNING: Could not show mouse cursor.\n");
	}

	// init event queue
	_event_queue = al_create_event_queue();
	if (!_event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		_panic = true;
		return;
	}

	al_register_event_source(_event_queue, al_get_keyboard_event_source());
	al_register_event_source(_event_queue, al_get_mouse_event_source());
	al_register_event_source(_event_queue, al_get_display_event_source(_display));
}

void Main::loop() {
	while (true) {
		handleEvents();
		if (_shutdown)
			break;
		updateFps();
		drawFrame();
	}
}

void Main::handleEvents() {
	ALLEGRO_EVENT ev;
	while (!_shutdown && al_get_next_event(_event_queue, &ev)) {
		// select event type
		switch (ev.type) {
		case ALLEGRO_EVENT_KEY_DOWN:
			break;
		case ALLEGRO_EVENT_KEY_UP:
			break;
		case ALLEGRO_EVENT_KEY_CHAR:
			handleKeyEvent(ev.keyboard.keycode);
			break;

		case ALLEGRO_EVENT_MOUSE_AXES:
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			if (ev.mouse.button == 1)
				handleClickEvent(ev.mouse.x, ev.mouse.y);
			break;

		case ALLEGRO_EVENT_MOUSE_WARPED:
			break;
		case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
			break;
		case ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY:
			break;

		case ALLEGRO_EVENT_TIMER:
			break;

		case ALLEGRO_EVENT_DISPLAY_EXPOSE:
			break;
		case ALLEGRO_EVENT_DISPLAY_RESIZE:
			break;
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			_shutdown = true;
			break;
		case ALLEGRO_EVENT_DISPLAY_LOST:
		case ALLEGRO_EVENT_DISPLAY_FOUND:
		case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
		case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
		case ALLEGRO_EVENT_DISPLAY_ORIENTATION:

		default:
			break; // ignore everything we don't know
		} // select event type
	} // process all events
}

void Main::updateFps() {
	int64 new_us_counter = US_PER_TICK * al_get_timer_count(_timer);
	if (new_us_counter / 1000000 > _us_counter / 1000000) {
		_fps = _fps_counter;
		_fps_counter = 0;
	}
	_us_counter = new_us_counter;
}

void Main::handleKeyEvent(int key) {
	switch (key) {
		case ALLEGRO_KEY_ESCAPE:
			_shutdown = true;
			break;

		case ALLEGRO_KEY_R: {
			shared_ptr<Board> shared_board = Board::factoryStandard();
			Situation situation(move(*shared_board), PLAYER_WHITE);
			shared_board.reset();
			_game->reset(situation);
			_iter = --_game->history().end();
			break;
		}

		case ALLEGRO_KEY_O: {
			Orientation orient = _view->getOrientation();
			orient = (Orientation)(((int)orient + 1) % 4);
			_view->setOrientation(orient);
			break;
		}

		case ALLEGRO_KEY_B:
			if (_iter->index > 0)
				--_iter;
			break;

		case ALLEGRO_KEY_N:
			if (_iter->index < (int)_game->history().size() - 1)
				++_iter;
			break;
	} // switch
}

void Main::handleClickEvent(int x, int y) {
	const Situation &situation = _iter->situation;

	// which tile was clicked?
	Tile tile = _view->getTileAt(x, y);

	// click on the board
	if (situation.isInBound(tile)) {
		// clicked already selected piece
		if (tile == _selection) {
			_selection = situation.INVALID_TILE;
		}

		// clicked another self owned piece
		else if (situation[tile].player == situation.active_player()) {
			_selection = tile;
		}

		// clicked the second tile to make a move
		else {
			makeMove(_selection, tile);
			_selection = situation.INVALID_TILE;
		}

		return;
	}

	Type promoType = _view->getPromotionTypeAt(x, y);

	// click on promotion selector
	if(promoType != TYPE_NONE) {
		_promo_selection = promoType;
		return;
	}

	int button = _view->getButtonAt(x, y);

	// click on button
	if(button != BUTTON_NONE) {
		switch(button) {
		default:
			break;
		}
		return;
	}
}

void Main::makeMove(Tile src, Tile dst) {
	const Situation &situation = _iter->situation;

	// forget all moves made after the current one
	_game->seek(_iter->index);

	Rules rules;
	Action action = rules.examineMove(situation, src, dst, _promo_selection);
	if (!rules.isActionLegal(situation, action))
		return;

	_game->action(action);
	printf("%s: ", action.player == PLAYER_WHITE ? "white" : "black");
	printf("%c%c -> ", 'A' + action.src[0], '1' + action.src[1]);
	printf("%c%c", 'A' + action.dst[0], '1' + action.dst[1]);
	printf(" id:%d", action.type);
	printf(" (promote to %d)\n", action.promotion);

	_bot->update(action);
	action = _bot->next_action();

	if (!rules.isActionLegal(_game->current_situation(), action))
		printf("bot is retarded.\n");

	_game->action(action);
	printf("%s: ", action.player == PLAYER_WHITE ? "white" : "black");
	printf("%c%c -> ", 'A' + action.src[0], '1' + action.src[1]);
	printf("%c%c", 'A' + action.dst[0], '1' + action.dst[1]);
	printf(" id:%d", action.type);
	printf(" (promote to %d)\n", action.promotion);

	_bot->update(action);

	_iter = --_game->history().end();
}

void Main::drawFrame() {
	al_set_target_backbuffer(al_get_current_display());
	_view->draw(0.0, 0.0, _iter->situation, _selection, _promo_selection);
	auto bg = al_color_name("black");
	auto fg = al_color_name("white");
	al_draw_filled_rectangle(440, 0, 640, 490, bg);
	int lineno = 0;
	int file = _iter->situation.en_passant_file();
	al_draw_textf(_font, fg, 460, 20 + lineno++ * 32, 0, "fps: %3d\n", _fps);
	al_draw_textf(_font, fg, 460, 20 + lineno++ * 32, 0, "en passant:\n");
	al_draw_textf(_font, fg, 460, 20 + lineno++ * 32, 0, "%d\n", file);
	al_draw_textf(_font, fg, 460, 20 + lineno++ * 32, 0, "\n");
	al_draw_textf(_font, fg, 460, 20 + lineno++ * 32, 0, "turn:\n");
	al_draw_textf(_font, fg, 460, 20 + lineno++ * 32, 0, "%d\n", _iter->index);
	al_flip_display();
	++_fps_counter;
}
