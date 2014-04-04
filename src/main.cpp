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

	void initialize(int argc, char **argv);
	void finilize();
	int main(int argc, char **argv);

private:
	ALLEGRO_DISPLAY *_display = nullptr;
	ALLEGRO_EVENT_QUEUE *_event_queue = nullptr;
	ALLEGRO_TIMER *_timer = nullptr;
	ALLEGRO_FONT *_font = nullptr;

	Game *_game = nullptr;
	Game::HistoryConstIter _iter;
	View *_view = nullptr;

	bool _panic = nullptr;
};

int main(int argc, char** argv) {
	Main main;
	return main.main(argc, argv);
}

void Main::initialize(int argc, char **argv) {
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

	al_init_font_addon();
	if (!al_init_ttf_addon()) {
		fprintf(stderr, "failed to initialize ttf font addon!\n");
		_panic = true;
		return;
	}

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

	// show systom mouse cursor
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

void Main::finilize() {
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
	al_uninstall_system();
}

int Main::main(int argc, char **argv)
{
	_display = nullptr;
	_event_queue = nullptr;
	_timer = nullptr;
	_game = nullptr;
	_view = nullptr;
	_font = nullptr;
	_panic = false;

	initialize(argc, argv);

	if (_panic) {
		fprintf(stderr, "Initialization failed.  Terminating program...\n");
		finilize();
		exit(-1);
	}

	shared_ptr<Board> shared_board = Board::factoryStandard();
	const Board DEFAULT_BOARD = move(*shared_board);
	shared_board.reset();
	const Situation DEFAULT_SITUATION(DEFAULT_BOARD, PLAYER_WHITE);
	Game &game = *_game;
	game.reset(DEFAULT_SITUATION);
	_iter = --game.history().end();

	al_start_timer(_timer);

    SpeedyBot speedy_bot;
	Bot &bot = speedy_bot;
	bot.reset(DEFAULT_SITUATION);

	bool shutdown = false;
	int fps_counter = 0;
	int fps = 0;
	int64 us_counter = 0;
	int64 new_us_counter;
	Tile selection = Tile(-1, -1);
	while (true) {
		new_us_counter = US_PER_TICK * al_get_timer_count(_timer);

		// fetch all events in the queue and process them in order
		ALLEGRO_EVENT ev;
		while (!shutdown && al_get_next_event(_event_queue, &ev)) {
			// select event type
			switch (ev.type) {
			case ALLEGRO_EVENT_KEY_DOWN:
				break;
			case ALLEGRO_EVENT_KEY_UP:
				break;
			case ALLEGRO_EVENT_KEY_CHAR: {
				auto key = ev.keyboard.keycode;
				if (key == ALLEGRO_KEY_ESCAPE) {
					shutdown = true;
				} else if (key == ALLEGRO_KEY_R) {
					game.reset(DEFAULT_SITUATION);
					_iter = --game.history().end();
				} else if (key == ALLEGRO_KEY_O) {
					Orientation orient = _view->getOrientation();
					orient = (Orientation)(((int)orient + 1) % 4);
					_view->setOrientation(orient);
				} else if (key == ALLEGRO_KEY_B) {
					if (_iter->index > 0)
						--_iter;
				} else if (key == ALLEGRO_KEY_N) {
					if (_iter->index < (int)game.history().size() - 1)
						++_iter;
				}
				break;
			}

			case ALLEGRO_EVENT_MOUSE_AXES:
				break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_UP: {
				const Situation &situation = _iter->situation;
				ALLEGRO_MOUSE_STATE mouse;
				al_get_mouse_state(&mouse);
				Tile tile = _view->getTileAt(mouse.x, mouse.y);
				Player active_player = situation.active_player();
				if (ev.mouse.button == 1) {
					// click outside the board
					if (!situation.isInBound(tile)) {
						selection = situation.INVALID_TILE;
						goto NEXT_EVENT;
					}

					// clicked already selected piece
					if (tile == selection) {
						selection = situation.INVALID_TILE;
						goto NEXT_EVENT;
					}

					// clicked another self owned piece
					if (situation[tile].player == active_player) {
						selection = tile;
						goto NEXT_EVENT;
					}

					// tried to make a move
					{
						game.seek(_iter->index);

						Rules rules;
						Action action;
						action = rules.examineMove(situation, selection, tile);
						bool is_legal = rules.isActionLegal(situation, action);
						if (is_legal) {
							game.action(action);
							printf("%s: ", action.player == PLAYER_WHITE ? "white" : "black");
							printf("%c%c -> ", 'A' + action.src[0], '1' + action.src[1]);
							printf("%c%c", 'A' + action.dst[0], '1' + action.dst[1]);
							printf(" id:%d", action.type);
							printf(" (promote to %d)\n", action.promotion);
							selection = situation.INVALID_TILE;

							bot.update(action);
							action = bot.next_action();

							if (!rules.isActionLegal(game.current_situation(), action))
                                printf("bot is retarded.\n");

							game.action(action);
							printf("%s: ", action.player == PLAYER_WHITE ? "white" : "black");
							printf("%c%c -> ", 'A' + action.src[0], '1' + action.src[1]);
							printf("%c%c", 'A' + action.dst[0], '1' + action.dst[1]);
							printf(" id:%d", action.type);
							printf(" (promote to %d)\n", action.promotion);
							selection = situation.INVALID_TILE;
							bot.update(action);

							_iter = --game.history().end();

							goto NEXT_EVENT;
						}
					}
				}
				break;
			}

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
				shutdown = true;
				break;
			case ALLEGRO_EVENT_DISPLAY_LOST:
			case ALLEGRO_EVENT_DISPLAY_FOUND:
			case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
			case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
			case ALLEGRO_EVENT_DISPLAY_ORIENTATION:

			default:
				break; // ignore everything we don't know
			} // select event type
		NEXT_EVENT:;
		} // process all events

		if (shutdown)
			break;

		// whenever a full second has passed
		if (new_us_counter / 1000000 > us_counter / 1000000) {
			fps = fps_counter;
			fps_counter = 0;
		}
		us_counter = new_us_counter;

		al_set_target_backbuffer(al_get_current_display());
		_view->draw(0.0, 0.0, _iter->situation, selection);
		auto bg = al_color_name("black");
		auto fg = al_color_name("white");
		al_draw_filled_rectangle(440, 0, 640, 490, bg);
		int lineno = 0;
		int file = _iter->situation.en_passant_file();
		al_draw_textf(_font, fg, 460, 20 + lineno++ * 32, 0, "fps: %3d\n", fps);
		al_draw_textf(_font, fg, 460, 20 + lineno++ * 32, 0, "en passant:\n");
		al_draw_textf(_font, fg, 460, 20 + lineno++ * 32, 0, "%d\n", file);
		al_draw_textf(_font, fg, 460, 20 + lineno++ * 32, 0, "\n");
		al_draw_textf(_font, fg, 460, 20 + lineno++ * 32, 0, "turn:\n");
		al_draw_textf(_font, fg, 460, 20 + lineno++ * 32, 0, "%d\n", _iter->index);
		al_flip_display();
		++fps_counter;
	} // main loop

	finilize();

	// exit program success
	return 0;
}
