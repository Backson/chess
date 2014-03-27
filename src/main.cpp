#include <cstdio>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include "View.hpp"
#include "Board.hpp"
#include "Action.hpp"
#include "Piece.hpp"
#include "Rules.hpp"

static const int TIMER_BPS = 1000;
static const int64 US_PER_TICK = 1e6 / TIMER_BPS;

struct ProgramContext {
	ALLEGRO_DISPLAY *display;
	ALLEGRO_EVENT_QUEUE *event_queue;
	ALLEGRO_TIMER *timer;

	Position *position;
	View *view;

	bool panic;
};

void initialize(int argc, char** argv, ProgramContext *pc)
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
        pc->position = new Position();
        auto w = pc->position->width();
        auto h = pc->position->height();
        pc->view = new View(w, h, WHITE_AT_THE_BOTTOM, 20);
    }

	// init display
	{
        al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_OPENGL);
        int w = pc->view->getPanelWidthPixels();
        int h = pc->view->getPanelHeightPixels();
        pc->display = al_create_display(w, h);
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
	if (pc->view) {
		delete pc->view;
		pc->view = NULL;
	}
	if (pc->position) {
		delete pc->position;
		pc->position = NULL;
	}
	View::deinitialize();
	if (pc->timer) {
		al_destroy_timer(pc->timer);
		pc->timer = NULL;
	}
	if (pc->display) {
		al_destroy_display(pc->display);
		pc->display = NULL;
	}
	if (pc->event_queue) {
		al_destroy_event_queue(pc->event_queue);
		pc->event_queue = NULL;
	}
	al_uninstall_system();
}

int main(int argc, char** argv)
{
	//printTypeInformation();

	ProgramContext pc;
	pc.display = NULL;
	pc.event_queue = NULL;
	pc.timer = NULL;
	pc.position = NULL;
	pc.view = NULL;
	pc.panic = false;

	initialize(argc, argv, &pc);

	if (pc.panic) {
		fprintf(stderr, "Initialization failed.  Terminating program...\n");
		finilize(&pc);
		exit(-1);
	}

    const Board DEFAULT_BOARD = Board::factoryStandard();
    const Position DEFAULT_POSITION = Position(DEFAULT_BOARD, PLAYER_WHITE);
    Position &position = *pc.position;
    position = DEFAULT_POSITION;

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
					position = DEFAULT_POSITION;
				} else if (key == ALLEGRO_KEY_O) {
					float w = pc.view->getBoardWidth();
					float h = pc.view->getBoardHeight();
					Orientation orient = pc.view->getOrientation();
					float b = pc.view->getBorderSizePixels();
					orient = (Orientation)(((int)orient + 1) % 4);
					delete pc.view;
					pc.view = new View(w, h, orient, b);
				}
				break;
			}

			case ALLEGRO_EVENT_MOUSE_AXES:
				break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_UP: {
			    ALLEGRO_MOUSE_STATE mouse;
			    al_get_mouse_state(&mouse);
				Tile tile = pc.view->getTileAt(mouse.x, mouse.y);
				Player active_player = position.active_player();
				if (ev.mouse.button == 1) {
                    // click outside the board
					if (!position.isInBound(tile)) {
						selection = position.INVALID_TILE;
						goto NEXT_EVENT;
					}

					// clicked already selected piece
					if (tile == selection) {
						selection = position.INVALID_TILE;
						goto NEXT_EVENT;
					}

                    // clicked another self owned piece
					if (position[tile].player ==  active_player) {
						selection = tile;
						goto NEXT_EVENT;
					}

                    // tried to make a move
                    {
                        Rules rules;
                        Action action;
                        action = rules.examineMove(*pc.position, selection, tile);
                        bool is_legal = rules.isActionLegal(*pc.position, action);
                        if (is_legal) {
                            pc.position->action(action);
                            printf("%s: ", action.player == PLAYER_WHITE ? "white" : "black");
                            printf("%c%c -> ", 'A' + action.src[0], '1' + action.src[1]);
                            printf("%c%c", 'A' + action.dst[0], '1' + action.dst[1]);
                            printf(" id:%d", action.type);
                            printf(" (promote to %d)\n", action.promotion);
                            selection = position.INVALID_TILE;
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
		pc.view->draw(0.0, 0.0, *pc.position, selection);
		al_flip_display();
		++fps_counter;
	} // main loop

	finilize(&pc);

	// exit program success
	return 0;
}
