#include "Board.h"
#include "Zobrist.h"

void Board::ApplyMove(const Move& move) {
    // Apply castling move
    if ((*this)(move.from) == Piece::WHITE_KING && move.from == Square{ 0, 4 }) {
        if (move.to == Square{ 0, 2 }) {
            //Move rook
            (*this)({ 0, 0 }) = Piece::NO_PIECE;
            (*this)({ 0, 3 }) = Piece::WHITE_ROOK;
        }
        if (move.to == Square{ 0, 6 }) {
            //Move rook
            (*this)({ 0, 7 }) = Piece::NO_PIECE;
            (*this)({ 0, 5 }) = Piece::WHITE_ROOK;
        }
    }
    if ((*this)(move.from) == Piece::BLACK_KING && move.from == Square{ 7, 4 }) {
        if (move.to == Square{ 0, 2 }) {
            //Move rook
            (*this)({ 7, 0 }) = Piece::NO_PIECE;
            (*this)({ 7, 3 }) = Piece::BLACK_ROOK;
        }
        if (move.to == Square{ 0, 6 }) {
            //Move rook
            (*this)({ 7, 7 }) = Piece::NO_PIECE;
            (*this)({ 7, 5 }) = Piece::BLACK_ROOK;
        }
    }

    auto piece = (*this)(move.from);

    hash ^= GetZobristHash(move.from, piece);
    hash ^= GetZobristHash(move.to, piece);
    if (!IsEmpty(move.to)) {
        //hash ^= GetZobristHash(move.to, (*this)(move.to));
    }

    (*this)(move.to) = piece;
    (*this)(move.from) = Piece::NO_PIECE;


    if (move.to.rank == 7 && (*this)(move.to) == Piece::WHITE_PAWN)
        (*this)(move.to) = Piece::WHITE_QUEEN;
    if (move.to.rank == 0 && (*this)(move.to) == Piece::BLACK_PAWN)
        (*this)(move.to) = Piece::BLACK_QUEEN;
}
