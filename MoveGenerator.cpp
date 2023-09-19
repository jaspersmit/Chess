#include <iostream>

#include "MoveGenerator.h"

#define GEN_MOVE(target) moves.push_back({ from, target })

void GeneratePawnMoves(const Board& board, std::vector<Move>& moves, Square from) {
    auto pushDir = (board.GetTurn() == Color::WHITE) ? Direction{1, 0} : Direction{-1, 0};
    auto up1 = from.Add(pushDir);
    if (board.IsEmpty(up1)) {
        if (from.rank == 6) {
            // Special treat promotion
        }
        moves.push_back({ from, up1 });
        int8_t startRank = (board.GetTurn() == Color::WHITE) ? 1 : 6;
        if (from.rank == startRank) {
            auto up2 = up1.Add(pushDir);
            if (board(up2) == Piece::NO_PIECE)
                GEN_MOVE(up2);
        }
    }

    // Capturing moves
    if (from.file > 0) {
        auto left = up1.Add(0, -1);
        if (board.IsOtherPlayer(left)) {
            GEN_MOVE(left);
        }
    }
    if (from.file < 7) {
        auto right = up1.Add(0, 1);
        if (board.IsOtherPlayer(right)) {
            GEN_MOVE(right);
        }
    }
}

Direction knightMoves[8] = {
    {-1, -2},
    {-2, -1},
    {1, -2},
    {2, -1},
    {1, 2},
    {2, 1},
    {-1, 2},
    {-2, 1}
};

void GenerateKnightMoves(const Board& board, std::vector<Move>& moves, Square from) {
    for (int i = 0; i < 8; i++) {
        auto to = from.Add(knightMoves[i]);
        if (to.IsValid() && !board.IsCurrentPlayer(to)) {
            GEN_MOVE(to);
        }
    }
}

Direction bishopDirections[] = {
    {-1, -1},
    {1, -1},
    {1, 1},
    {-1, 1}
};

void GenerateBishopMoves(const Board& board, std::vector<Move>& moves, Square from) {
    for (int i = 0; i < 4; i++) {
        auto direction = bishopDirections[i];
        auto to = from;
        while (true) {
            to = to.Add(direction);
            if (!to.IsValid()) break;
            if (board.IsCurrentPlayer(to)) break;
            GEN_MOVE(to);
            if (board.IsOtherPlayer(to)) break;
        }
    }
}

Direction rookDirections[] = {
    {0, 1},
    {1, 0},
    {-1, 0},
    {0, -1}
};

void GenerateRookMoves(const Board& board, std::vector<Move>& moves, Square from) {
    for (int i = 0; i < 4; i++) {
        auto direction = rookDirections[i];
        auto to = from;
        while (true) {
            to = to.Add(direction);
            if (!to.IsValid()) break;
            if (board.IsCurrentPlayer(to)) break;
            GEN_MOVE(to);
            if (board.IsOtherPlayer(to)) break;
        }
    }
}

void GenerateQueenMoves(const Board& board, std::vector<Move>& moves, Square from) {
    GenerateBishopMoves(board, moves, from);
    GenerateRookMoves(board, moves, from);
}

Direction kingDirections[] = {
    {0, 1},
    {1, 0},
    {-1, 0},
    {0, -1},
    {-1, -1},
    {1, -1},
    {1, 1},
    {-1, 1}
};

void GenerateKingMoves(const Board& board, std::vector<Move>& moves, Square from) {
    for (int i = 0; i < 8; i++) {
        Square to = from.Add(kingDirections[i]);
        if (to.IsValid() && !board.IsCurrentPlayer(to)) {
            GEN_MOVE(to);
        }
    }
    //Castling
    if (from == Square{ 0, 4 }) {
        if (board({ 0, 0 }) == Piece::WHITE_ROOK
            && board({ 0, 1 }) == Piece::NO_PIECE
            && board({ 0, 2 }) == Piece::NO_PIECE
            && board({ 0, 3 }) == Piece::NO_PIECE) {
            Square to = { 0, 2 };
            GEN_MOVE(to);
        }
        if (board({ 0, 7 }) == Piece::WHITE_ROOK
            && board({ 0, 5 }) == Piece::NO_PIECE
            && board({ 0, 6 }) == Piece::NO_PIECE) {
            Square to = { 0, 6 };
            GEN_MOVE(to);
        }
    }
    if (from == Square{ 7, 4 }) {
        if (board({ 7, 0 }) == Piece::BLACK_ROOK
            && board({ 7, 1 }) == Piece::NO_PIECE
            && board({ 7, 2 }) == Piece::NO_PIECE
            && board({ 7, 3 }) == Piece::NO_PIECE) {
            Square to = { 7, 2 };
            GEN_MOVE(to);
        }
        if (board({ 7, 7 }) == Piece::BLACK_ROOK
            && board({ 7, 5 }) == Piece::NO_PIECE
            && board({ 7, 6 }) == Piece::NO_PIECE) {
            Square to = { 7, 6 };
            GEN_MOVE(to);
        }
    }
}

void GenerateMoves(const Board& board, std::vector<Move>& moves) {
    for (int8_t r = 7; r >= 0; r--) {
        for (int8_t f = 0; f < 8; f++) {
            auto square = Square{ r, f };
            if (board.GetTurn() == Color::WHITE) {
                switch (board(square)) {
                case Piece::WHITE_PAWN:
                    GeneratePawnMoves(board, moves, square);
                    break;
                case Piece::WHITE_ROOK:
                    GenerateRookMoves(board, moves, square);
                    break;
                case Piece::WHITE_KNIGHT:
                    GenerateKnightMoves(board, moves, square);
                    break;
                case Piece::WHITE_BISHOP:
                    GenerateBishopMoves(board, moves, square);
                    break;
                case Piece::WHITE_QUEEN:
                    GenerateQueenMoves(board, moves, square);
                    break;
                case Piece::WHITE_KING:
                    GenerateKingMoves(board, moves, square);
                    break;
                }
            }
            else {
                switch (board(square)) {
                case Piece::BLACK_PAWN:
                    GeneratePawnMoves(board, moves, square);
                    break;
                case Piece::BLACK_ROOK:
                    GenerateRookMoves(board, moves, square);
                    break;
                case Piece::BLACK_KNIGHT:
                    GenerateKnightMoves(board, moves, square);
                    break;
                case Piece::BLACK_BISHOP:
                    GenerateBishopMoves(board, moves, square);
                    break;
                case Piece::BLACK_QUEEN:
                    GenerateQueenMoves(board, moves, square);
                    break;
                case Piece::BLACK_KING:
                    GenerateKingMoves(board, moves, square);
                    break;
                }
            }
        }
    }
}

auto IsInCheck(Board& board) -> bool {
    board.SwitchTurn();
    std::vector<Move> moves;
    GenerateMoves(board, moves);
    for (auto& move : moves) {
        if (board.GetTurn() == Color::WHITE && board(move.to) == Piece::BLACK_KING ||
            board.GetTurn() == Color::BLACK && board(move.to) == Piece::WHITE_KING) {
            board.SwitchTurn();
            return true;
        }
    }
    board.SwitchTurn();
    return false;
}

