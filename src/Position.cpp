#include "Position.hpp"

#include "Board.hpp"
#include "Action.hpp"
#include "Piece.hpp"
#include "Rules.hpp"

// LIFECYCLE

Position::Position() :
	_board(Board::factoryStandard())
{
	_game_state = STATE_START;
	_en_passant_chance_file = -1;
	_castling_chances[0][KINGSIDE]  = true;
	_castling_chances[0][QUEENSIDE] = true;
	_castling_chances[1][KINGSIDE]  = true;
	_castling_chances[1][QUEENSIDE] = true;
	_half_turn_counter = 0;
}

Position::Position(const Position& other) :
	_board(other._board)
{
	_game_state = other._game_state;
	_en_passant_chance_file = other._en_passant_chance_file;
	_castling_chances[0][KINGSIDE]  = other._castling_chances[0][KINGSIDE];
	_castling_chances[0][QUEENSIDE] = other._castling_chances[0][QUEENSIDE];
	_castling_chances[1][KINGSIDE]  = other._castling_chances[1][KINGSIDE];
	_castling_chances[1][QUEENSIDE] = other._castling_chances[1][QUEENSIDE];
	_half_turn_counter = other._half_turn_counter;
}

Position& Position::operator=(const Position& other)
{
	if (this == &other) return *this; // handle self assignment

	_board = other._board;
	_game_state = other._game_state;
	_en_passant_chance_file = other._en_passant_chance_file;
	_castling_chances[0][KINGSIDE]  = other._castling_chances[0][KINGSIDE];
	_castling_chances[0][QUEENSIDE] = other._castling_chances[0][QUEENSIDE];
	_castling_chances[1][KINGSIDE]  = other._castling_chances[1][KINGSIDE];
	_castling_chances[1][QUEENSIDE] = other._castling_chances[1][QUEENSIDE];
	_half_turn_counter = other._half_turn_counter;

	return *this;
}

Position::Position(const Board& board, GameState game_state, int en_passant_chance_file) :
	_board(board),
	_game_state(game_state),
	_en_passant_chance_file(en_passant_chance_file)
{
	_castling_chances[0][KINGSIDE]  = false;
	_castling_chances[0][QUEENSIDE] = false;
	_castling_chances[1][KINGSIDE]  = false;
	_castling_chances[1][QUEENSIDE] = false;
	_half_turn_counter = 0;
}

// OPERATORS

bool Position::operator==(const Position& other) const {
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

    // ignore half turn counter
//	if (_half_turn_counter != other._half_turn_counter)
//		return false;

	return true;
}
bool Position::operator!=(const Position& other) const {
	return !operator==(other);
}

// ACCESS

const Board& Position::board() const {
	return _board;
}

Board& Position::board() {
	return _board;
}

GameState Position::game_state() const {
	return _game_state;
}

bool Position::is_playing() const {
	return _game_state == STATE_WHITE_TURN || _game_state == STATE_BLACK_TURN;
}

Player Position::active_player() const {
	if (_game_state == STATE_WHITE_TURN)
		return PLAYER_WHITE;
	if (_game_state == STATE_BLACK_TURN)
		return PLAYER_BLACK;
	return PLAYER_WHITE;
}

int Position::half_turn_counter() const {
    return _half_turn_counter;
}

int8 Position::en_passant_chance_file() const {
	return _en_passant_chance_file;
}

bool Position::can_castle(CastlingType type, Player player) const {
	int player_i = player == PLAYER_WHITE ? 0 : 1;
	return _castling_chances[player_i][type];
}

// OPERATIONS

void Position::start() {
	if (_game_state == STATE_START)
		_game_state = STATE_WHITE_TURN;
}

void Position::action(const Action& a) {
	if (!is_playing())
		return;

    bool is_pawn_move = _board[a.src].type == TYPE_PAWN;
    bool is_capture = a.type == CAPTURE_PIECE;
    if (is_pawn_move || is_capture)
        _half_turn_counter = 0;
    else
        ++_half_turn_counter;

	if (_board[a.src].type == TYPE_KING) {
		_castling_chances[a.player][KINGSIDE] = false;
		_castling_chances[a.player][QUEENSIDE] = false;
	}

	if (_board[a.src].type == TYPE_PAWN && (a.dst - a.src).norm2() == 4) {
		Tile left = a.dst + Tile((int8)-1, (int8)0);
		Tile right = a.dst + Tile((int8)1, (int8)0);
		bool pawnLeft = _board.isInBound(left) && _board[left].type == TYPE_PAWN && _board[left].player != a.player;
		bool pawnRight = _board.isInBound(right) && _board[right].type == TYPE_PAWN && _board[right].player != a.player;
		if(pawnLeft || pawnRight) {
			_en_passant_chance_file = a.dst[0];
		} else {
			_en_passant_chance_file = -1;
		}
	} else {
		_en_passant_chance_file = -1;
	}

	if (a.type == CASTLING) {
        int8 home_row = a.player == PLAYER_WHITE ? 0 : _board.height() - 1;
		int8 sx = (a.dst - a.src)[0] > 0 ? +1 : -1;
		Tile rook_src = Tile((int8)(sx > 0 ? _board.width() - 1 : 0), home_row);
		Tile rook_dst = a.src + Tile(sx, (int8)0);
		_board.movePiece(a.src, a.dst);
		_board.movePiece(rook_src, rook_dst);
	} else {
		for (int player = 0; player < 2; ++player)
			for (int castling = 0; castling < 2; ++castling) {
				int8 x = castling == KINGSIDE ? _board.width() - 1 : 0;
				int8 y = player == PLAYER_WHITE ? 0 : _board.height() - 1;
				if (a.src == Tile(x, y) || a.dst == Tile(x, y))
					_castling_chances[player][castling] = false;
			}
		
		_board.movePiece(a.src, a.dst);
	}
	
	if (a.promotion != TYPE_NONE) {
		_board[a.dst] = Piece{_board[a.dst].player, a.promotion};
	}
	
	if (a.type == EN_PASSANT) {
		_board.removePiece(Tile(a.dst[0], a.src[1]));
	}
	
	_game_state = _game_state == STATE_WHITE_TURN ? STATE_BLACK_TURN : STATE_WHITE_TURN;
}
