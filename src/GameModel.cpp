/** @file GameModel.cpp
 *
 */

#include "GameModel.h"

#include "Board.h"
#include "Move.h"
#include "Rules.h"

// LIFECYCLE

GameModel::GameModel()
{
    board = new Board(Board::factoryStandard());
    gameState = STATE_START;
    enPassantChanceFile = -1;
}

GameModel::~GameModel()
{
    delete board;
}

GameModel::GameModel(const GameModel& other)
{
    board = new Board;

    *this->board = *other.board;
    this->gameState = other.gameState;
    this->enPassantChanceFile = other.enPassantChanceFile;
}

GameModel& GameModel::operator=(const GameModel& other)
{
    if (this == &other) return *this; // handle self assignment

    *this->board = *other.board;
    this->gameState = other.gameState;
    this->enPassantChanceFile = other.enPassantChanceFile;

    return *this;
}

GameModel::GameModel(const Board& board, GameState gameState, Coord enPassantChanceFile)
    : gameState(gameState), enPassantChanceFile(enPassantChanceFile)
{
    this->board = new Board;
    *this->board = board;
    // nothing
}

// OPERATORS

bool GameModel::operator==(const GameModel& other) const
{
    if (this->gameState != other.gameState) return false;
    if (this->enPassantChanceFile != other.enPassantChanceFile) return false;

    if (*this->board != *other.board) return false;

    return true;
}
bool GameModel::operator!=(const GameModel& other) const
{
    return !operator==(other);
}

// ACCESS

const Board& GameModel::getBoard() const
{
    return *board;
}
Board& GameModel::getBoard()
{
    return *board;
}
GameState GameModel::getGameState() const
{
    return gameState;
}
bool GameModel::isPlaying() const
{
    return gameState == STATE_WHITE_TURN || gameState == STATE_BLACK_TURN;
}

bool GameModel::isMoveLegal(const Square& src, const Square& dest) const
{
    Move m = Rules::examineMove(this, src, dest);
    return Rules::isMoveLegal(this, m);
}


// OPERATIONS

int GameModel::start()
{
    if (gameState == STATE_START)
    {
        gameState = STATE_WHITE_TURN;
        return 0; // game was started
    }
    else
    {
        return 1; // game is already running
    }
}

int GameModel::move(const Move& m)
{
    if (!Rules::isMoveLegal(this, m)) return 1;

    board->removePiece(m.dest);
    board->movePiece(m.src, m.dest);

    gameState = gameState == STATE_WHITE_TURN ? STATE_BLACK_TURN : STATE_WHITE_TURN;

    //changeSignal(*this);

    return 0;
}
