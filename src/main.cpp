#include <cstdio>

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

static const int TIMER_BPS = 1000;
static const int64 US_PER_TICK = 1e6 / TIMER_BPS;

struct ProgramContext {
	ALLEGRO_DISPLAY *display;
	ALLEGRO_EVENT_QUEUE *event_queue;
	ALLEGRO_TIMER *timer;
	ALLEGRO_FONT *font;

	Game *game;
	Game::HistoryConstIter iter;
	View *view;

	bool panic;
};

void initialize(int argc, char **argv, ProgramContext *pc)
{
	// init allegro
	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		pc->panic = true;
		return;
	}
	if (!al_install_keyboard()) {
		fprintf(stderr, "failed to initialize keyboard driver!\n");
		pc->panic = true;
		return;
	}
	if (!al_install_mouse()) {
		fprintf(stderr, "failed to initialize mouse driver!\n");
		pc->panic = true;
		return;
	}
	if (!al_init_image_addon()) {
		fprintf(stderr, "failed to initialize image loading addon!\n");
		pc->panic = true;
		return;
	}
	if (!al_init_primitives_addon()) {
		fprintf(stderr, "failed to initialize primitives drawing addon!\n");
		pc->panic = true;
		return;
	}

	al_init_font_addon();
	if (!al_init_ttf_addon()) {
		fprintf(stderr, "failed to initialize ttf font addon!\n");
		pc->panic = true;
		return;
	}

	pc->font = al_load_font("Bevan.ttf", -32, ALLEGRO_TTF_MONOCHROME);
	if (!pc->font) {
		pc->panic = true;
		return;
	}

	pc->timer = al_create_timer(ALLEGRO_BPS_TO_SECS(TIMER_BPS));
	if (!pc->timer) {
		pc->panic = true;
		return;
	}

	if (View::initialize()) {
		fprintf(stderr, "failed to initialize view!\n");
		pc->panic = true;
		return;
	}

	// init game model
	{
		Situation situation(Position::factoryStandard(), PLAYER_WHITE);
		pc->game = new Game(situation);
		const Situation &current = pc->game->current_situation();
		auto w = current.width();
		auto h = current.height();
		pc->view = new View(w, h, WHITE_AT_THE_BOTTOM, 20);
	}

	// init display
	{
		al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_OPENGL);
		int w = pc->view->getPanelWidthPixels();
		int h = pc->view->getPanelHeightPixels();
		pc->display = al_create_display(w + 200, h + 50);
		if (!pc->display) {
			fprintf(stderr, "failed to create display!\n");
			pc->panic = true;
			return;
		}
	}

	// show systom mouse cursor
	if (!al_show_mouse_cursor(pc->display)) {
		fprintf(stderr, "WARNING: Could not show mouse cursor.\n");
	}

	// init event queue
	pc->event_queue = al_create_event_queue();
	if (!pc->event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		pc->panic = true;
		return;
	}

	al_register_event_source(pc->event_queue, al_get_keyboard_event_source());
	al_register_event_source(pc->event_queue, al_get_mouse_event_source());
	al_register_event_source(pc->event_queue, al_get_display_event_source(pc->display));
}

void finilize(ProgramContext *pc) {
	if (pc->font) {
		al_destroy_font(pc->font);
		pc->font = nullptr;
	}
	if (pc->view) {
		delete pc->view;
		pc->view = nullptr;
	}
	if (pc->game) {
		delete pc->game;
		pc->game = nullptr;
	}
	View::deinitialize();
	if (pc->timer) {
		al_destroy_timer(pc->timer);
		pc->timer = nullptr;
	}
	if (pc->display) {
		al_destroy_display(pc->display);
		pc->display = nullptr;
	}
	if (pc->event_queue) {
		al_destroy_event_queue(pc->event_queue);
		pc->event_queue = nullptr;
	}
	al_uninstall_system();
}

int main(int argc, char** argv)
{
	//printTypeInformation();

	ProgramContext pc;
	pc.display = nullptr;
	pc.event_queue = nullptr;
	pc.timer = nullptr;
	pc.game = nullptr;
	pc.view = nullptr;
	pc.font = nullptr;
	pc.panic = false;

	initialize(argc, argv, &pc);

	if (pc.panic) {
		fprintf(stderr, "Initialization failed.  Terminating program...\n");
		finilize(&pc);
		exit(-1);
	}

	const Board DEFAULT_BOARD = Board::factoryStandard();
	const Situation DEFAULT_SITUATION(DEFAULT_BOARD, PLAYER_WHITE);
	Game &game = *pc.game;
	game.reset(DEFAULT_SITUATION);
	pc.iter = --game.history().end();

	al_start_timer(pc.timer);

	bool shutdown = false;
	int fps_counter = 0;
	int fps = 0;
	int64 us_counter = 0;
	int64 new_us_counter;
	Tile selection = Tile(-1, -1);
	while (true) {
		new_us_counter = US_PER_TICK * al_get_timer_count(pc.timer);

		// fetch all events in the queue and process them in order
		ALLEGRO_EVENT ev;
		while (!shutdown && al_get_next_event(pc.event_queue, &ev)) {
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
					pc.iter = --game.history().end();
				} else if (key == ALLEGRO_KEY_O) {
					Orientation orient = pc.view->getOrientation();
					orient = (Orientation)(((int)orient + 1) % 4);
					pc.view->setOrientation(orient);
				} else if (key == ALLEGRO_KEY_B) {
					if (pc.iter->index > 0)
						--pc.iter;
				} else if (key == ALLEGRO_KEY_N) {
					if (pc.iter->index < (int)pc.game->history().size() - 1)
						++pc.iter;
				}
				break;
			}

			case ALLEGRO_EVENT_MOUSE_AXES:
				break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_UP: {
				const Situation &situation = pc.iter->situation;
				ALLEGRO_MOUSE_STATE mouse;
				al_get_mouse_state(&mouse);
				Tile tile = pc.view->getTileAt(mouse.x, mouse.y);
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
					if (situation[tile].player ==  active_player) {
						selection = tile;
						goto NEXT_EVENT;
					}

					// tried to make a move
					{
						game.seek(pc.iter->index);

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

							pc.iter = --game.history().end();

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
			printf("fps: %3d\n", fps);
		}
		us_counter = new_us_counter;

		al_set_target_backbuffer(al_get_current_display());
		pc.view->draw(0.0, 0.0, pc.iter->situation, selection);
		auto bg = al_color_name("black");
		auto fg = al_color_name("white");
		al_draw_filled_rectangle(440, 0, 640, 490, bg);
		int lineno = 0;
		int file = game.current_situation().en_passant_file();
		al_draw_textf(pc.font, fg, 460, 20 + lineno++ * 32, 0, "en passant:\n");
		al_draw_textf(pc.font, fg, 460, 20 + lineno++ * 32, 0, "%d\n", file);
		al_draw_textf(pc.font, fg, 460, 20 + lineno++ * 32, 0, "\n");
		al_draw_textf(pc.font, fg, 460, 20 + lineno++ * 32, 0, "turn:\n");
		al_draw_textf(pc.font, fg, 460, 20 + lineno++ * 32, 0, "%d\n", pc.iter->index);
		al_flip_display();
		++fps_counter;
	} // main loop

	finilize(&pc);

	// exit program success
	return 0;
}
