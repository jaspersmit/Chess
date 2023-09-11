#include <algorithm>
#include <vector>

#include "Board.h"
#include "Move.h"
#include "Piece.h"

void OrderMoves(const Board& board, std::vector<Move>& moves, std::vector<int>& indices) {
    std::vector<int> moveScores(moves.size());
    for (size_t i = 0; i < moves.size(); i++) {
        auto move = moves[i];

        int score = 0;

        switch (board(move.to)) {
        case Piece::BLACK_KING:
            score += MAX_SCORE;
            break;
        case Piece::BLACK_QUEEN:
            score += 90;
            break;
        case Piece::BLACK_ROOK:
            score += 50;
            break;
        case Piece::BLACK_KNIGHT:
        case Piece::BLACK_BISHOP:
            score += 30;
            break;
        case Piece::BLACK_PAWN:
            score += 10;
            break;
        }

        switch (board(move.from)) {
        case Piece::WHITE_KING:
            score += 1;
            break;
        case Piece::WHITE_QUEEN:
            score += 9;
            break;
        case Piece::WHITE_ROOK:
            score += 5;
            break;
        case Piece::WHITE_KNIGHT:
        case Piece::WHITE_BISHOP:
            score += 3;
            break;
        }
        moveScores[i] = score;
    }


    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
        return moveScores[a] > moveScores[b];
        });
}