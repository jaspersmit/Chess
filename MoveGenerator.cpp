#include "MoveGenerator.h"

#define GEN_MOVE(target) moves.push_back({ from, target })

void GeneratePawnMoves(const Board& board, std::vector<Move>& moves, Square from) {
    auto up1 = from.Add(1, 0);
    if (board.IsEmpty(up1)) {
        if (from.rank == 6) {
            // Special treat promotion
        }
        moves.push_back({ from, up1 });
        if (from.rank == 1) {
            auto up2 = up1.Add(1, 0);
            if (board(up2) == Piece::NO_PIECE)
                GEN_MOVE(up2);
        }
    }

    // Capturing moves
    if (from.file > 0) {
        auto left = up1.Add(0, -1);
        if (board.HasEnemy(left)) {
            GEN_MOVE(left);
        }
    }
    if (from.file < 7) {
        auto right = up1.Add(0, 1);
        if (board.HasEnemy(right)) {
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
        if (to.IsValid() && !board.HasFriend(to)) {
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
            if (board.HasFriend(to)) break;
            GEN_MOVE(to);
            if (board.HasEnemy(to)) break;
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
            if (board.HasFriend(to)) break;
            GEN_MOVE(to);
            if (board.HasEnemy(to)) break;
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
        if (to.IsValid() && !board.HasFriend(to)) {
            GEN_MOVE(to);
        }
    }
    //Castling
    if (from == Square{ 0, 4 }) {
        if (board({ 0, 0 }) == Piece::MY_ROOK
            && board({ 0, 1 }) == Piece::NO_PIECE
            && board({ 0, 2 }) == Piece::NO_PIECE
            && board({ 0, 3 }) == Piece::NO_PIECE) {
            Square to = { 0, 2 };
            GEN_MOVE(to);
        }
        if (board({ 0, 7 }) == Piece::MY_ROOK
            && board({ 0, 5 }) == Piece::NO_PIECE
            && board({ 0, 6 }) == Piece::NO_PIECE) {
            Square to = { 0, 6 };
            GEN_MOVE(to);
        }
    }
}

void GenerateMoves(const Board& board, std::vector<Move>& moves) {
    for (int8_t r = 7; r >= 0; r--) {
        for (int8_t f = 0; f < 8; f++) {
            auto square = Square{ r, f };
            switch (board(square)) {
            case Piece::MY_PAWN:
                GeneratePawnMoves(board, moves, square);
                break;
            case Piece::MY_ROOK:
                GenerateRookMoves(board, moves, square);
                break;
            case Piece::MY_KNIGHT:
                GenerateKnightMoves(board, moves, square);
                break;
            case Piece::MY_BISHOP:
                GenerateBishopMoves(board, moves, square);
                break;
            case Piece::MY_QUEEN:
                GenerateQueenMoves(board, moves, square);
                break;
            case Piece::MY_KING:
                GenerateKingMoves(board, moves, square);
                break;
            }
        }
    }
}