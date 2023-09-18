#include "Board.h"
#include "Zobrist.h"

void Board::ApplyMove(const Move& move) {
    // Apply castling move
    if ((*this)(move.from) == Piece::WHITE_KING && move.from == Square{ 0, 4 }) {
        if (move.to == Square{ 0, 2 }) {
            //Move rook
            SetSquare({ 0, 0 }, Piece::NO_PIECE);
            SetSquare({ 0, 3 }, Piece::WHITE_ROOK);
        }
        if (move.to == Square{ 0, 6 }) {
            //Move rook
            SetSquare({ 0, 7 }, Piece::NO_PIECE);
            SetSquare({ 0, 5 }, Piece::WHITE_ROOK);
        }
    }
    if ((*this)(move.from) == Piece::BLACK_KING && move.from == Square{ 7, 4 }) {
        if (move.to == Square{ 0, 2 }) {
            //Move rook
            SetSquare({ 7, 0 }, Piece::NO_PIECE);
            SetSquare({ 7, 3 }, Piece::BLACK_ROOK);
        }
        if (move.to == Square{ 0, 6 }) {
            //Move rook
            SetSquare({ 7, 7 }, Piece::NO_PIECE);
            SetSquare({ 7, 5 }, Piece::BLACK_ROOK);
        }
    }

    auto piece = (*this)(move.from);

    SetSquare(move.from, Piece::NO_PIECE);
    SetSquare(move.to, piece);

    if (move.to.rank == 7 && (*this)(move.to) == Piece::WHITE_PAWN)
        SetSquare(move.to, Piece::WHITE_QUEEN);
    if (move.to.rank == 0 && (*this)(move.to) == Piece::BLACK_PAWN)
        SetSquare(move.to, Piece::BLACK_QUEEN);
}
