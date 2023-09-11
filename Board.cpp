#include "Board.h"

void Board::ApplyMove(const Move& move) {
    // Apply castling move
    if ((*this)(move.from) == Piece::MY_KING && move.from == Square{ 0, 4 }) {
        if (move.to == Square{ 0, 2 }) {
            //Move rook
            (*this)({ 0, 0 }) = Piece::NO_PIECE;
            (*this)({ 0, 3 }) = Piece::MY_ROOK;
        }
        if (move.to == Square{ 0, 6 }) {
            //Move rook
            (*this)({ 0, 7 }) = Piece::NO_PIECE;
            (*this)({ 0, 5 }) = Piece::MY_ROOK;
        }
    }

    (*this)(move.to) = (*this)(move.from);
    (*this)(move.from) = Piece::NO_PIECE;
    if (move.to.rank == 7 && (*this)(move.to) == Piece::MY_PAWN)
        (*this)(move.to) = Piece::MY_QUEEN;
}
