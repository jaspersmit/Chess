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
        if (move.to == Square{ 7, 2 }) {
            //Move rook
            SetSquare({ 7, 0 }, Piece::NO_PIECE);
            SetSquare({ 7, 3 }, Piece::BLACK_ROOK);
        }
        if (move.to == Square{ 7, 6 }) {
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

void ParseBoard(Board& board, const std::string& str) {
    if (str.length() != 64) {
        std::cout << "Could not parse board";
    }

    for (int i = 0; i < 64; i++) {
        char c = str[i];
        auto piece = Piece::NO_PIECE;

        switch (c) {
        case 'p':
            piece = Piece::WHITE_PAWN;
            break;
        case 'r':
            piece = Piece::WHITE_ROOK;
            break;
        case 'n':
            piece = Piece::WHITE_KNIGHT;
            break;
        case 'b':
            piece = Piece::WHITE_BISHOP;
            break;
        case 'k':
            piece = Piece::WHITE_KING;
            break;
        case 'q':
            piece = Piece::WHITE_QUEEN;
            break;
        case 'P':
            piece = Piece::BLACK_PAWN;
            break;
        case 'R':
            piece = Piece::BLACK_ROOK;
            break;
        case 'N':
            piece = Piece::BLACK_KNIGHT;
            break;
        case 'B':
            piece = Piece::BLACK_BISHOP;
            break;
        case 'K':
            piece = Piece::BLACK_KING;
            break;
        case 'Q':
            piece = Piece::BLACK_QUEEN;
            break;
        }
        int8_t rank = 7 - (i / 8);
        int8_t file = i % 8;
        board({ rank, file }) = piece;
    }
}