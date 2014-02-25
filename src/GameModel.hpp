#ifndef GAME_MODEL_HPP
#define GAME_MODEL_HPP

#include "Action.hpp"
#include "Board.hpp"

enum GameState
{
    STATE_START,
    STATE_WHITE_TURN,
    STATE_BLACK_TURN,
    STATE_WHITE_WIN,
    STATE_BLACK_WIN,
    STATE_DRAW,
};

class GameModel
{
public:
    // LIFECYCLE
    GameModel();
    GameModel(const GameModel& other);
    GameModel& operator=(const GameModel& other);

    GameModel(const Board& board, GameState game_state, int en_passant_chance_file = -1);

    // OPERATORS
    bool operator==(const GameModel& other) const;
    bool operator!=(const GameModel& other) const;

    // ACCESS
    const Board& getBoard() const;
    Board& getBoard();
    GameState getGameState() const;
    bool isPlaying() const;
	Player getActivePlayer() const;
	
	int getEnPassantChanceFile() const;
	bool canCastle(CastlingType type, Player player) const;

    // OPERATIONS
    void start();
    void action(const Action& action);

private:
    Board _board;
    GameState _game_state;

    int _en_passant_chance_file;
	bool _castling_chances[2][2]; // ...[player][castling_type]
};

#endif // GAME_MODEL_HPP
