/** @file GameModel.cpp
 *
 */

#include "GameModel.h"

#include "Board.h"
#include "Move.h"
#include "Square.h"
#include "Piece.h"
#include "Rules.h"

// LIFECYCLE

GameModel::GameModel() :
	_board(Board::factoryStandard())
{
    _game_state = STATE_START;
    _en_passant_file_chance = -1;
	_castling_chances[0][KINGSIDE]  = true;
	_castling_chances[0][QUEENSIDE] = true;
	_castling_chances[1][KINGSIDE]  = true;
	_castling_chances[1][QUEENSIDE] = true;
}

GameModel::GameModel(const GameModel& other) :
	_board(other._board)
{
    _game_state = other._game_state;
    _en_passant_file_chance = other._en_passant_file_chance;
	_castling_chances[0][KINGSIDE]  = other._castling_chances[0][KINGSIDE];
	_castling_chances[0][QUEENSIDE] = other._castling_chances[0][QUEENSIDE];
	_castling_chances[1][KINGSIDE]  = other._castling_chances[1][KINGSIDE];
	_castling_chances[1][QUEENSIDE] = other._castling_chances[1][QUEENSIDE];
}

GameModel& GameModel::operator=(const GameModel& other)
{
    if (this == &other) return *this; // handle self assignment

    _board = other._board;
    _game_state = other._game_state;
    _en_passant_file_chance = other._en_passant_file_chance;
	_castling_chances[0][KINGSIDE]  = other._castling_chances[0][KINGSIDE];
	_castling_chances[0][QUEENSIDE] = other._castling_chances[0][QUEENSIDE];
	_castling_chances[1][KINGSIDE]  = other._castling_chances[1][KINGSIDE];
	_castling_chances[1][QUEENSIDE] = other._castling_chances[1][QUEENSIDE];

    return *this;
}

GameModel::GameModel(const Board& board, GameState game_state, Coord en_passant_file_chance) :
	_board(board),
	_game_state(game_state),
	_en_passant_file_chance(en_passant_file_chance)
{
	_castling_chances[0][KINGSIDE]  = false;
	_castling_chances[0][QUEENSIDE] = false;
	_castling_chances[1][KINGSIDE]  = false;
	_castling_chances[1][QUEENSIDE] = false;
    // nothing
}

// OPERATORS

bool GameModel::operator==(const GameModel& other) const {
    if (_board != other._board)
		return false;
    if (_game_state != other._game_state)
		return false;
    if (_en_passant_file_chance != other._en_passant_file_chance)
		return false;
    if (_castling_chances[0][KINGSIDE] != other._castling_chances[0][KINGSIDE])
		return false;
    if (_castling_chances[0][QUEENSIDE] != other._castling_chances[0][QUEENSIDE])
		return false;
    if (_castling_chances[1][KINGSIDE] != other._castling_chances[1][KINGSIDE])
		return false;
    if (_castling_chances[1][QUEENSIDE] != other._castling_chances[1][QUEENSIDE])
		return false;

    return true;
}
bool GameModel::operator!=(const GameModel& other) const {
    return !operator==(other);
}

// ACCESS

const Board& GameModel::getBoard() const {
    return _board;
}

Board& GameModel::getBoard() {
    return _board;
}

GameState GameModel::getGameState() const {
    return _game_state;
}

bool GameModel::isPlaying() const {
    return _game_state == STATE_WHITE_TURN || _game_state == STATE_BLACK_TURN;
}

Player GameModel::getActivePlayer() const {
	if (_game_state == STATE_WHITE_TURN)
		return PLAYER_WHITE;
	if (_game_state == STATE_BLACK_TURN)
		return PLAYER_BLACK;
	return PLAYER_WHITE;
}

Coord GameModel::getEnPassantFile() const {
	return _en_passant_file_chance;
}

bool GameModel::canCastle(CastlingType type, Player player) const {
	int player_i = player == PLAYER_WHITE ? 0 : 1;
	return _castling_chances[player_i][type];
}

// OPERATIONS

void GameModel::start() {
    if (_game_state == STATE_START)
        _game_state = STATE_WHITE_TURN;
}

void GameModel::move(const Move& m) {
    if (!isPlaying())
		return;

	if (m.castling_flag) {
		Coord file_d = m.dest.getFile() - m.src.getFile();
		bool kingside = file_d > 0;
		Coord file_step = kingside ? +1 : -1;
		Coord rank_rook = m.src.getRank();
		Coord file_rook = m.src.getFile() + (kingside ? +3 : -4);
		Square square_rook_src = Square(rank_rook, file_rook);
		Square square_rook_dest = Square(rank_rook, m.src.getFile() + file_step);
		_board.movePiece(m.src, m.dest);
		_board.movePiece(square_rook_src, square_rook_dest);
		int player_i = m.player == PLAYER_WHITE ? 0 : 1;
		_castling_chances[player_i][KINGSIDE] = false;
		_castling_chances[player_i][QUEENSIDE] = false;
	} else {
		if (_board.getPiece(m.src).getType() == TYPE_KING) {
			int player_i = m.player == PLAYER_WHITE ? 0 : 1;
			_castling_chances[player_i][KINGSIDE] = false;
			_castling_chances[player_i][QUEENSIDE] = false;
		}
		if (_board.getPiece(m.src).getType() == TYPE_ROOK) {
			int player_i = m.player == PLAYER_WHITE ? 0 : 1;
			Coord rank_rook = m.player == PLAYER_WHITE ? 0 : _board.getHeight() - 1;
			if (m.src == Square(rank_rook, _board.getWidth() - 1))
				_castling_chances[player_i][KINGSIDE] = false;
			else if (m.src == Square(rank_rook, _board.getWidth() - 1))
				_castling_chances[player_i][QUEENSIDE] = false;
		}
		_board.movePiece(m.src, m.dest);
	}
	
    _game_state = _game_state == STATE_WHITE_TURN ? STATE_BLACK_TURN : STATE_WHITE_TURN;
}
