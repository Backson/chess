#include "GameModel.hpp"

#include "Board.hpp"
#include "Action.hpp"
#include "Piece.hpp"
#include "Rules.hpp"

// LIFECYCLE

GameModel::GameModel() :
	_board(Board::factoryStandard())
{
    _game_state = STATE_START;
    _en_passant_chance_file = -1;
	_castling_chances[0][KINGSIDE]  = true;
	_castling_chances[0][QUEENSIDE] = true;
	_castling_chances[1][KINGSIDE]  = true;
	_castling_chances[1][QUEENSIDE] = true;
}

GameModel::GameModel(const GameModel& other) :
	_board(other._board)
{
    _game_state = other._game_state;
    _en_passant_chance_file = other._en_passant_chance_file;
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
    _en_passant_chance_file = other._en_passant_chance_file;
	_castling_chances[0][KINGSIDE]  = other._castling_chances[0][KINGSIDE];
	_castling_chances[0][QUEENSIDE] = other._castling_chances[0][QUEENSIDE];
	_castling_chances[1][KINGSIDE]  = other._castling_chances[1][KINGSIDE];
	_castling_chances[1][QUEENSIDE] = other._castling_chances[1][QUEENSIDE];

    return *this;
}

GameModel::GameModel(const Board& board, GameState game_state, int en_passant_chance_file) :
	_board(board),
	_game_state(game_state),
	_en_passant_chance_file(en_passant_chance_file)
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
    if (_en_passant_chance_file != other._en_passant_chance_file)
		return false;
    if (_castling_chances[PLAYER_WHITE][KINGSIDE] != other._castling_chances[PLAYER_WHITE][KINGSIDE])
		return false;
    if (_castling_chances[PLAYER_WHITE][QUEENSIDE] != other._castling_chances[PLAYER_WHITE][QUEENSIDE])
		return false;
    if (_castling_chances[PLAYER_BLACK][KINGSIDE] != other._castling_chances[PLAYER_BLACK][KINGSIDE])
		return false;
    if (_castling_chances[PLAYER_BLACK][QUEENSIDE] != other._castling_chances[PLAYER_BLACK][QUEENSIDE])
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

int GameModel::getEnPassantChanceFile() const {
	return _en_passant_chance_file;
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

void GameModel::action(const Action& a) {
    if (!isPlaying())
		return;

	int8 home_row = a.player == PLAYER_WHITE ? 0 : _board.height() - 1;

	if (a.type == CASTLING) {
		int8 sx = (a.dst - a.src)[0] > 0 ? +1 : -1;
		Tile king_src = Tile((int8)(_board.width() / 2), home_row);
		Tile king_dst = Tile((int8)(_board.width() / 2 + 2 * sx), home_row);
		Tile rook_src = Tile((int8)(sx > 0 ? _board.width() - 1 : 0), home_row);
		Tile rook_dst = king_src + Tile(sx, (int8)0);
		_board.movePiece(king_src, king_dst);
		_board.movePiece(rook_src, rook_dst);
		_castling_chances[a.player][KINGSIDE] = false;
		_castling_chances[a.player][QUEENSIDE] = false;
	} else {
		if (_board[a.src].type == TYPE_KING) {
			_castling_chances[a.player][KINGSIDE] = false;
			_castling_chances[a.player][QUEENSIDE] = false;
		}
		for (int player = 0; player < 2; ++player)
			for (int castling = 0; castling < 2; ++castling) {
				int8 x = castling == KINGSIDE ? _board.width() - 1 : 0;
				int8 y = player == PLAYER_WHITE ? 0 : _board.height() - 1;
				if (a.src == Tile(x, y))
					_castling_chances[player][castling] = false;
			}
		
		_board.movePiece(a.src, a.dst);
	}
	
    _game_state = _game_state == STATE_WHITE_TURN ? STATE_BLACK_TURN : STATE_WHITE_TURN;
}
