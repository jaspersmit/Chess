#include <algorithm>
#include <vector>

#include "Board.h"
#include "Move.h"
#include "MoveOrder.h"
#include "Piece.h"

Killers killers[MAX_KILLERS_DEPTH];


namespace {
    auto GetPieceValue(Piece piece) -> int {
        switch (piece) {
        case Piece::WHITE_KING:
        case Piece::BLACK_KING:
            return 1000;
        case Piece::WHITE_QUEEN:
        case Piece::BLACK_QUEEN:
            return 9;
        case Piece::WHITE_ROOK:
        case Piece::BLACK_ROOK:
            return 5;
        case Piece::WHITE_KNIGHT:
        case Piece::WHITE_BISHOP:
        case Piece::BLACK_KNIGHT:
        case Piece::BLACK_BISHOP:
            return 3;
        case Piece::WHITE_PAWN:
        case Piece::BLACK_PAWN:
            return 1;
        }
        return 0;
    }
}

void OrderMoves(const Board& board, MoveList& moves, std::array<int, 128>& indices, Move hashMove, Killers& killers) {
    std::array<int, 128> moveScores;
    for (int i = 0; i < moves.GetNumMoves(); i++) {
        auto move = moves.GetMove(i);
        int score = 0;
        if (move == hashMove) {
            score += 1000;
        }

        if (!board.IsEmpty(move.to)) {
            score += 100 + GetPieceValue(board(move.to)) - GetPieceValue(board(move.from));
        }

        if (killers.Match(move)) {
            score += 50;
        }

        else {
            switch (board(move.from)) {
            case Piece::WHITE_KING:
            case Piece::BLACK_KING:
                break;
            case Piece::WHITE_QUEEN:
            case Piece::BLACK_QUEEN:
                score += 1;
            case Piece::WHITE_ROOK:
            case Piece::BLACK_ROOK:
                score += 2;
            case Piece::WHITE_KNIGHT:
            case Piece::WHITE_BISHOP:
            case Piece::BLACK_KNIGHT:
            case Piece::BLACK_BISHOP:
                score += 4;
            case Piece::WHITE_PAWN:
            case Piece::BLACK_PAWN:
                score += 3;
            }
        }
        moveScores[i] = score;
    }


    std::iota(indices.begin(), indices.begin() + moves.GetNumMoves(), 0);
    std::sort(indices.begin(), indices.begin() + moves.GetNumMoves(), [&](size_t a, size_t b) {
        return moveScores[a] > moveScores[b];
        });
}