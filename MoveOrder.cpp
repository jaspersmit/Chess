#include <algorithm>
#include <vector>

#include "Board.h"
#include "Move.h"
#include "Piece.h"

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
    }
}

void OrderMoves(const Board& board, std::vector<Move>& moves, std::vector<int>& indices, Move hashMove) {
    std::vector<int> moveScores(moves.size());
    for (size_t i = 0; i < moves.size(); i++) {
        auto move = moves[i];
        int score = 0;
        if (move == hashMove) {
            score += 1000;
        }

        if (!board.IsEmpty(move.to)) {
            score += 100 + GetPieceValue(board(move.to)) - GetPieceValue(board(move.from));
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


    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
        return moveScores[a] > moveScores[b];
        });
}