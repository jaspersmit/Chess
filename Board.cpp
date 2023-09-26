#include <vector>

#include "Board.h"
#include "Zobrist.h"

struct HistoricMove {
    Square from;
    Square to;
    Piece capturedPiece;
    bool promotion;
};

std::vector<HistoricMove> history;

Board theBoard;

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

void DoMove(const Move& move) {
    // Apply castling move
    Piece king;
    Piece rook;
    Piece pawn;
    Piece queen;
    int8_t kingRank;
    if (theBoard.GetTurn() == Color::WHITE) {
        king = Piece::WHITE_KING;
        rook = Piece::WHITE_ROOK;
        pawn = Piece::WHITE_PAWN;
        queen = Piece::WHITE_QUEEN;
        kingRank = 0;
    }
    else {
        king = Piece::BLACK_KING;
        rook = Piece::BLACK_ROOK;
        pawn = Piece::BLACK_PAWN;
        queen = Piece::BLACK_QUEEN;
        kingRank = 7;
    }

    if (theBoard(move.from) == king && move.from == Square{ kingRank, 4 }) {
        if (move.to == Square{ kingRank, 2 }) {
            //Move rook
            theBoard.SetSquare({ kingRank, 0 }, Piece::NO_PIECE);
            theBoard.SetSquare({ kingRank, 3 }, rook);
        }
        if (move.to == Square{ 0, 6 }) {
            //Move rook
            theBoard.SetSquare({ kingRank, 7 }, Piece::NO_PIECE);
            theBoard.SetSquare({ kingRank, 5 }, rook);
        }
    }

    auto piece = theBoard(move.from);
    auto capturedPiece = theBoard(move.to);

    theBoard.SetSquare(move.from, Piece::NO_PIECE);
    theBoard.SetSquare(move.to, piece);

    auto promotion = false;
    if (move.to.rank == (7 - kingRank) && theBoard(move.to) == pawn) {
        theBoard.SetSquare(move.to, queen);
        promotion = true;
    }

    history.emplace_back(move.from, move.to, capturedPiece, promotion);
}

void UndoMove() {
    auto move = history.back();
    history.pop_back();

    // Apply castling move
    Piece king;
    Piece rook;
    Piece pawn;
    int8_t kingRank;

    if (theBoard.GetTurn() == Color::WHITE) {
        king = Piece::WHITE_KING;
        rook = Piece::WHITE_ROOK;
        pawn = Piece::WHITE_PAWN;
        kingRank = 0;
    }
    else {
        king = Piece::BLACK_KING;
        rook = Piece::BLACK_ROOK;
        pawn = Piece::BLACK_PAWN;
        kingRank = 7;
    }

    if (theBoard(move.to) == king && move.to == Square{ kingRank, 4 }) {
        if (move.from == Square{ kingRank, 2 }) {
            //Move rook
            theBoard.SetSquare({ kingRank, 0 }, rook);
            theBoard.SetSquare({ kingRank, 3 }, Piece::NO_PIECE);
        }
        if (move.to == Square{ 0, 6 }) {
            //Move rook
            theBoard.SetSquare({ kingRank, 7 }, rook);
            theBoard.SetSquare({ kingRank, 5 }, Piece::NO_PIECE);
        }
    }

    auto piece = theBoard(move.to);

    theBoard.SetSquare(move.from, piece);
    theBoard.SetSquare(move.to, move.capturedPiece);

    if (move.promotion)
        theBoard.SetSquare(move.from, pawn);
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