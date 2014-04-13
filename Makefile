CC=g++
CFLAGS=-c -Wall -std=gnu++0x -g -I"."
LDFLAGS=-L"." -lboost_thread -lboost_system -lallegro -lallegro_color -lallegro_primitives -lallegro_image -lallegro_font -lallegro_ttf
SOURCES=Action.cpp Board.cpp Bot.cpp compare.cpp Game.cpp main.cpp MoveCache.cpp Piece.cpp Position.cpp RandomBot.cpp Rules.cpp Situation.cpp SpeedyBot.cpp vec.cpp View.cpp zobrist.cpp
OBJECTS=$(SOURCES:.cpp=.o)
SRC_FILES=$(addprefix src/,$(SOURCES))
OBJ_FILES=$(addprefix obj/,$(OBJECTS))
EXECUTABLE=chess

all: $(SRC_FILES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) $(LDFLAGS) -o $@

obj/%.o : src/%.cpp
	$(CC) $(CFLAGS) $< -o $@
