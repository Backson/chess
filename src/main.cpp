#include <stdio.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include "View.hpp"
#include "Board.hpp"
#include "Action.hpp"
#include "Piece.hpp"
#include "Rules.hpp"

struct ProgramContext {
	ALLEGRO_DISPLAY *display;
	ALLEGRO_EVENT_QUEUE *event_queue;
	
	GameModel *model;
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
 
	// install keyboard
	if (!al_install_keyboard()) {
		fprintf(stderr, "failed to initialize keyboard driver!\n");
		pc->panic = true;
		return;
	}

	// install mouse
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
	
	if (View::initialize()) {
		fprintf(stderr, "failed to initialize view!\n");
		pc->panic = true;
		return;
	}
	
	pc->model = new GameModel();
	pc->view = new View(8, 8, WHITE_AT_THE_BOTTOM, 20);
	
	// init display
	al_set_new_display_flags(ALLEGRO_WINDOWED);
	//al_set_new_display_option(ALLEGRO_COLOR_SIZE, 24, ALLEGRO_REQUIRE);
	pc->display = al_create_display(pc->view->getPanelWidthPixels(), pc->view->getPanelHeightPixels());
	if (!pc->display) {
		fprintf(stderr, "failed to create display!\n");
		pc->panic = true;
		return;
	}

	// show systom mouse cursor
	if (!al_show_mouse_cursor(pc->display)) {
		fprintf(stderr, "WARNING: Could not show mouse cursor.  Cursor might be invisible.\n");
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
	if (pc->model) {
		delete pc->model;
		pc->model = NULL;
	}
	View::deinitialize();
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
	pc.model = NULL;
	pc.view = NULL;
	pc.panic = false;

	initialize(argc, argv, &pc);

	if (pc.panic) {
		fprintf(stderr, "Initialization failed.  Terminating program...\n");
		exit(-1);
	}
	
	pc.model->start();
	
	bool shutdown = false;
	Tile selection = Tile((int8)-1, (int8)-1);
	ALLEGRO_MOUSE_STATE mouse;
	while (true) {
		// fetch all events in the queue and process them in order
		ALLEGRO_EVENT ev;
		while (!shutdown && al_get_next_event(pc.event_queue, &ev)) {
			al_get_mouse_state(&mouse);
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
					*pc.model = GameModel();
					pc.model->start();
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
				const Board &board = pc.model->getBoard();
				Tile tile = pc.view->getTileAt(mouse.x, mouse.y);
				if (ev.mouse.button == 1) {
					if (!board.isInBound(tile) || tile == selection) {
						selection = board.INVALID_TILE;
						goto NEXT_EVENT;
					}
					
					Rules rules;
					if (board.isInBound(selection)) {
						Action action;
						action = rules.examineMove(*pc.model, selection, tile);
						bool is_legal = rules.isActionLegal(*pc.model, action);
						if (is_legal) {
							pc.model->action(action);
							printf("%s: ", action.player == PLAYER_WHITE ? "white" : "black");
							printf("%c%c -> ", 'A' + action.src[0], '1' + action.src[1]);
							printf("%c%c", 'A' + action.dst[0], '1' + action.dst[1]);
							printf(" id:%d", action.type);
							printf(" (promote to %d)\n", action.promotion);
							selection = board.INVALID_TILE;
							goto NEXT_EVENT;
						}
					}
					//if (rules.hasLegalMove(*pc.model, tile))
					else if(board.isInBound(selection)
						|| board.piece(tile).player ==  pc.model->getActivePlayer())
						selection = tile;
					else
						selection = board.INVALID_TILE;
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
		
		al_set_target_backbuffer(al_get_current_display());
		pc.view->draw(0.0, 0.0, *pc.model, selection);
		al_flip_display();
		al_rest(0.01);
	} // main loop

	finilize(&pc);

	// exit program success
	return 0;
}
