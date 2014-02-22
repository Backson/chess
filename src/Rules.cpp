/** @file Rules.cpp
 *
 */

#include "Rules.h"

#include "Board.h"
#include "GameModel.h"
#include "Move.h"
#include "Piece.h"
#include "Square.h"
#include "types.h"

Move Rules::examineMove(const GameModel* gameModel, Square src, Square dest)
{
    Move m;

    m.src = src;
    m.dest = dest;

    const Board& board = gameModel->getBoard();

    m.player = board.getPiece(src).getPlayer();

    // check for promotion (assume queen)
    if (board.getPiece(src).getType() == TYPE_PAWN)
    {
        switch (m.player)
        {
        case PLAYER_WHITE:
            m.promotion = dest.getRank() == 7 ? TYPE_QUEEN : TYPE_NONE;
            break;
        case PLAYER_BLACK:
            m.promotion = dest.getRank() == 0 ? TYPE_QUEEN : TYPE_NONE;
            break;
        default:
            m.promotion = TYPE_NONE;
            break;
        }
    }
    else
    {
        m.promotion = TYPE_NONE;
    }

    m.en_passant_flag = false;
    m.capture_flag = board.getPiece(dest).getType() != TYPE_NONE;
    m.castling_flag = false;

    return m;
}

bool Rules::isMoveLegal(const GameModel* gameModel, Move move)
{
    DEFINE_PIECE_SHORTCUTS;

    // is it this players turn?
    switch (gameModel->getGameState())
    {
    case STATE_WHITE_TURN:
        if (move.player != PLAYER_WHITE)
            return false;
        break;
    case STATE_BLACK_TURN:
        if (move.player != PLAYER_BLACK)
            return false;
        break;
    default:
        return false;
    }

    // is there a piece on src?
    Piece src_piece = gameModel->getBoard().getPiece(move.src);
    if (src_piece == _)
        return false; // player wants to move an empty piece

    // is the piece owned by the player making the move?
    if (src_piece.getPlayer() != move.player)
        return false; // player wants to move his opponent's pieces

    // has the piece actually moved?
    if (move.src == move.dest)
        return false; // the piece must not stay on its square

    // check the different types of turns
    // CASTLING
    if (move.castling_flag == true)
    {
        if (move.capture_flag == true || move.en_passant_flag == true)
            return false; // invalid flags

        if (src_piece.getType() != TYPE_KING)
            return false; // only kings can castle

        /// @todo check, whether the rook or the king have previously moved

        /// @todo check, whether there're any pieces between king and rook

        /// @todo check, whether the king is currently in check
        /// @todo check, whether the king moves through an attacked square
        /// @todo check, whether the king ends up in check

        return true; // legal castling
    }

    // check, whether the target square is in the range of the piece
    if (!isSquareInRange(src_piece, move.src, move.dest, move.capture_flag))
        return false; // the piece can't go there

    bool pieceIsRider = src_piece.getType() != TYPE_KNIGHT;
    Piece dest_piece = gameModel->getBoard().getPiece(move.dest);

    // CAPTURE
    if (move.capture_flag == true)
    {
        // EN PASSANT
        if (move.en_passant_flag == true)
        {
            return false;
            /// @todo check for pawn to be captured

            /// @todo check, whether the pawn moved in the last half turn
        }

        // normal capture
        if (dest_piece == _)
        {
            return false; // you can only capture non-empty squares
        }
        else if (dest_piece.getPlayer() == src_piece.getPlayer())
        {
            return false; // you can only capture your opponent's pieces
        }
        else if (pieceIsRider)
        {
            if (gameModel->getBoard().getFirstBetween(move.src, move.dest) != _)
                return false;
        }

        return true;
    }

    // NORMAL TURN (no capture)
    if (move.en_passant_flag == true)
        return false; // no en passen without capture

    if (dest_piece != _)
        return false; // dest square is not empty

    if (pieceIsRider)
    {
        if (gameModel->getBoard().getFirstBetween(move.src, move.dest) != _)
            return false; // there is a piece in the way
    }

    return true;
}

bool Rules::isSquareInRange(Piece piece, Square src, Square dest, bool capture_flag)
{
    Coord d_rank = dest.getRank() - src.getRank();
    Coord d_file = dest.getFile() - src.getFile();

    // sort out moves, which end on the src square or outside the board
    if (d_rank == 0 && d_file == 0)
        return false;
    if (    dest.getRank() < 0 || dest.getRank() >= BOARD_HEIGHT
         || dest.getFile() < 0 || dest.getFile() >= BOARD_WIDTH     )
        return false;

    // treat pawns and other pieces separately
    switch (piece.getType())
    {
    case TYPE_KING:
    case TYPE_QUEEN:
    case TYPE_ROOK:
    case TYPE_BISHOP:
    case TYPE_KNIGHT:
        return isSquareInRange(piece.getType(), d_rank, d_file);
        break;
    case TYPE_PAWN:
        switch (piece.getPlayer())
        {
        case PLAYER_WHITE:
            if (d_rank <= 0) return false;
            if (capture_flag)
            {
                if (d_rank != 1) return false;
                if (d_file != 1 && d_file != -1) return false;
            }
            else
            {
                if (d_rank > (src.getRank() == 1 ? 2 : 1)) return false;
                if (d_file != 0) return false;
            }
            break;
        case PLAYER_BLACK:
            if (d_rank >= 0) return false;
            if (capture_flag)
            {
                if (d_rank != -1) return false;
                if (d_file != 1 && d_file != -1) return false;
            }
            else
            {
                if (d_rank < (src.getRank() == 6 ? -2 : -1)) return false;
                if (d_file != 0) return false;
            }
            break;
        default:
            return false; // error: pawn has no owner
        }
        break;
    case TYPE_NONE:
        return false; // non-exiting pieces can't move
        break;
    default:
        return false; // error: type of piece is none or unknown
        break;
    }

    return true; // all tests passed
}

bool Rules::isSquareInRange(Type type, Coord d_rank, Coord d_file)
{
    // sort out moves, which end on the src square
    if (d_rank == 0 && d_file == 0)
        return false;

    switch (type)
    {
    case TYPE_KING:
        if (d_rank > 1) return false;
        if (d_file > 1) return false;
        if (d_rank < -1) return false;
        if (d_file < -1) return false;
        break;
    case TYPE_QUEEN:
        if (    d_rank != 0 && d_file != 0
             && d_rank != d_file && d_rank != -d_file) return false;
        break;
    case TYPE_ROOK:
        if (d_rank != 0 && d_file != 0) return false;
        break;
    case TYPE_BISHOP:
        if (d_rank != d_file && d_rank != -d_file) return false;
        break;
    case TYPE_KNIGHT:
        if (d_rank * d_rank + d_file * d_file != 5)
            return false;
        break;
    case TYPE_PAWN:
        return false; // pawns can't be checked this way
    default:
        return false; // illegal piece type
    }
    return true; // all tests passed
}
