/** @file GameModel.h
 *
 */

#ifndef GameModel_h
#define GameModel_h

#include "types.h"
class Board;
class Move;
class Square;

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
    ~GameModel();
    GameModel(const GameModel& other);
    GameModel& operator=(const GameModel& other);

    GameModel(const Board& board, GameState gameState, Coord enPassantChanceFile = -1);

    // OPERATORS
    bool operator==(const GameModel& other) const;
    bool operator!=(const GameModel& other) const;

    // ACCESS
    const Board& getBoard() const;
    Board& getBoard();
    GameState getGameState() const;
    bool isPlaying() const;

    bool isMoveLegal(const Square& src, const Square& dest) const;

    // OPERATIONS
    int start();
    int move(const Move& move);

private:
    Board* board;
    GameState gameState;

    Coord enPassantChanceFile;
};

#endif // GameModel_h
