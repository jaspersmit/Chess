#include <vector>

#include "Board.h"
#include "Zobrist.h"

enum class SpecialMove {
    NORMAL_MOVE,
    INVALIDATE_CASTLING,
    INVALIDATE_CASTLING_KING_SIDE,
    INVALIDATE_CASTLING_QUEEN_SIDE
};

struct HistoricMove {
    Square from;
    Square to;
    Piece capturedPiece;
    bool promotion;
    SpecialMove specialMove;
};

std::vector<HistoricMove> history;

Board theBoard;

void DoMove(const Move& move) {
    auto specialMove = SpecialMove::NORMAL_MOVE;

    // Apply castling move
    Piece king;
    Piece rook;
    Piece pawn;
    Piece queen;
    int8_t kingRank;
    int8_t pawnRank;
    int8_t pawn2MoveRank;
    if (theBoard.GetTurn() == Color::WHITE) {
        king = Piece::WHITE_KING;
        rook = Piece::WHITE_ROOK;
        pawn = Piece::WHITE_PAWN;
        queen = Piece::WHITE_QUEEN;
        kingRank = 0;
        pawnRank = 1;
        pawn2MoveRank = 3;
    }
    else {
        king = Piece::BLACK_KING;
        rook = Piece::BLACK_ROOK;
        pawn = Piece::BLACK_PAWN;
        queen = Piece::BLACK_QUEEN;
        kingRank = 7;
        pawnRank = 6;
        pawn2MoveRank = 4;
    }

    if (theBoard(move.from) == king) {
        if (theBoard.HasCastlingRights(CastlingSide::KING))
            if (theBoard.HasCastlingRights(CastlingSide::QUEEN))
                specialMove = SpecialMove::INVALIDATE_CASTLING;
            else 
                specialMove = SpecialMove::INVALIDATE_CASTLING_KING_SIDE;
        else if (theBoard.HasCastlingRights(CastlingSide::QUEEN))
            specialMove = SpecialMove::INVALIDATE_CASTLING_QUEEN_SIDE;

        if (move.from == Square{ kingRank, 4 }) {
            if (move.to == Square{ kingRank, 2 }) {
                //Move rook
                theBoard.SetSquare({ kingRank, 0 }, Piece::NO_PIECE);
                theBoard.SetSquare({ kingRank, 3 }, rook);
            }
            if (move.to == Square{ kingRank, 6 }) {
                //Move rook
                theBoard.SetSquare({ kingRank, 7 }, Piece::NO_PIECE);
                theBoard.SetSquare({ kingRank, 5 }, rook);
            }
        }
    }

    if (theBoard(move.from) == rook) {
        if (move.from == Square{ kingRank, 0 } && theBoard.HasCastlingRights(CastlingSide::QUEEN)) {
            specialMove = SpecialMove::INVALIDATE_CASTLING_QUEEN_SIDE;
            theBoard.SetCastlingRights(CastlingSide::QUEEN, false);
        } 
        else if (move.from == Square{ kingRank, 7 } && theBoard.HasCastlingRights(CastlingSide::KING)) {
            specialMove = SpecialMove::INVALIDATE_CASTLING_KING_SIDE;
            theBoard.SetCastlingRights(CastlingSide::KING, false);
        }
    }

    if (theBoard(move.from) == pawn && move.from.rank == pawnRank && move.to.rank == pawn2MoveRank) {
        theBoard.SetEnpassentFile(move.to.file);
    }
    else {
        theBoard.SetEnpassentFile(INVALID_ENPASSENT_FILE);
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

    history.push_back({ move.from, move.to, capturedPiece, promotion, specialMove });
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

    if (theBoard(move.to) == king && move.from == Square{ kingRank, 4 }) {
        if (move.to == Square{ kingRank, 2 }) {
            //Move rook
            theBoard.SetSquare({ kingRank, 0 }, rook);
            theBoard.SetSquare({ kingRank, 3 }, Piece::NO_PIECE);
        }
        if (move.to == Square{ kingRank, 6 }) {
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

    switch (move.specialMove) {
    case SpecialMove::INVALIDATE_CASTLING:
        theBoard.SetCastlingRights(CastlingSide::QUEEN, true);
        theBoard.SetCastlingRights(CastlingSide::KING, true);
        break;
    case SpecialMove::INVALIDATE_CASTLING_KING_SIDE:
        theBoard.SetCastlingRights(CastlingSide::KING, true);
        break;
    case SpecialMove::INVALIDATE_CASTLING_QUEEN_SIDE:
        theBoard.SetCastlingRights(CastlingSide::QUEEN, true);
        break;
    }
}


void ParseBoard(Board& board, const std::string& str) {
    if (str.length() != 64) {
        std::cout << "Could not parse board";
    }

    board.Reset();

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
        board.SetSquare({ rank, file }, piece);
    }
}