/** @file GameModel.h
 *
 */

#ifndef GameModel_h
#define GameModel_h

#include "types.h"
#include "Board.h"
class Move;
class Square;

enum CastlingType {
	QUEENSIDE,
	KINGSIDE,
};

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

    GameModel(const Board& board, GameState game_state, Coord en_passant_file_chance = -1);

    // OPERATORS
    bool operator==(const GameModel& other) const;
    bool operator!=(const GameModel& other) const;

    // ACCESS
    const Board& getBoard() const;
    Board& getBoard();
    GameState getGameState() const;
    bool isPlaying() const;
	Player getActivePlayer() const;
	
	Coord getEnPassantFile() const;
	bool canCastle(CastlingType type, Player player) const;

    // OPERATIONS
    void start();
    void move(const Move& move);

private:
    Board _board;
    GameState _game_state;

    Coord _en_passant_file_chance;
	bool _castling_chances[2][2]; // ...[player][castling_type]
};

#endif // GameModel_h
