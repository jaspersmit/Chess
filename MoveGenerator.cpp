#include <iostream>

#include "MoveGenerator.h"

#define GEN_MOVE(target) moves.AddMove({ from, target })

void GeneratePawnMoves(const Board& board, MoveList& moves, Square from) {
    auto pushDir = (board.GetTurn() == Color::WHITE) ? Direction{1, 0} : Direction{-1, 0};
    uint8_t enPassantRank = (board.GetTurn() == Color::WHITE) ? 4 : 3;
    auto up1 = from.Add(pushDir);
    if (board.IsEmpty(up1)) {
        if (from.rank == 6) {
            // Special treat promotion
        }
        moves.AddMove({ from, up1 });
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
        if (board.IsOtherPlayer(left) ||
            (left.file == board.GetEnPassentFile() && from.rank == enPassantRank)) {
            GEN_MOVE(left);
        }
    }
    if (from.file < 7) {
        auto right = up1.Add(0, 1);
        if (board.IsOtherPlayer(right) ||
            (right.file == board.GetEnPassentFile() && from.rank == enPassantRank)) {
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

void GenerateKnightMoves(const Board& board, MoveList& moves, Square from) {
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

void GenerateBishopMoves(const Board& board, MoveList& moves, Square from) {
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

void GenerateRookMoves(const Board& board, MoveList& moves, Square from) {
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

void GenerateQueenMoves(const Board& board, MoveList& moves, Square from) {
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

class BitBoard {
public:

    static inline auto GetBit(Square s) -> uint64_t {
        return static_cast<uint64_t>(1) << (s.rank * 8 + s.file);
    }

    inline void Set(Square s) {
        bits |= GetBit(s);
    }

    inline auto IsSet(Square s) const -> bool {
        return bits & GetBit(s);
    }

    inline auto IsZero() const -> bool {
        return bits == 0;
    }

private:
    uint64_t bits = 0;
};

auto GenerateAttacks(const Board& board) -> BitBoard {
    BitBoard bitBoard;
    MoveList moves;
    GenerateMoves(board, moves, false);
    for (const auto& m : moves) {
        bitBoard.Set(m.to);
    }
    return bitBoard;
}

void GenerateKingMoves(const Board& board, MoveList& moves, Square from, bool generateCastling) {
    for (int i = 0; i < 8; i++) {
        Square to = from.Add(kingDirections[i]);
        if (to.IsValid() && !board.IsCurrentPlayer(to)) {
            GEN_MOVE(to);
        }
    }

    if (!generateCastling) return;
    //Castling
    int8_t r;
    Piece rook;
    
    if (board.GetTurn() == Color::WHITE) {
        r = 0;
        rook = Piece::WHITE_ROOK;
    }
    else {
        r = 7;
        rook = Piece::BLACK_ROOK;
    }
    BitBoard attacks;

    if (from == Square{ r, 4 }) {
        if (board({ r, 0 }) == rook
            && board({ r, 1 }) == Piece::NO_PIECE
            && board({ r, 2 }) == Piece::NO_PIECE
            && board({ r, 3 }) == Piece::NO_PIECE
            && board.HasCastlingRights(CastlingSide::QUEEN)) {

            Board nextBoard = board;
            nextBoard.SwitchTurn();
            attacks = GenerateAttacks(nextBoard);
            if (!attacks.IsSet({ r, 2 })
                && !attacks.IsSet({ r, 3 })
                && !attacks.IsSet({ r, 4 })) {
                Square to = { r, 2 };
                GEN_MOVE(to);
            }
        }
        if (board({ r, 7 }) == rook
            && board({ r, 5 }) == Piece::NO_PIECE
            && board({ r, 6 }) == Piece::NO_PIECE 
            && board.HasCastlingRights(CastlingSide::KING)) {

            if (attacks.IsZero()) {
                Board nextBoard = board;
                nextBoard.SwitchTurn();
                attacks = GenerateAttacks(nextBoard);
            }
            if (!attacks.IsSet({ r, 4 })
                && !attacks.IsSet({ r, 5 })
                && !attacks.IsSet({ r, 6 })) {
                Square to = { r, 6 };
                GEN_MOVE(to);
            }
        }
    }
}

void GenerateMoves(const Board& board, MoveList& moves, bool generateCastling) {
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
                    GenerateKingMoves(board, moves, square, generateCastling);
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
                    GenerateKingMoves(board, moves, square, generateCastling);
                    break;
                }
            }
        }
    }
}

auto IsInCheck(Board& board) -> bool {
    board.SwitchTurn();
    MoveList moves;
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

auto IsMoveValid(const Board& board, const Move& move) -> bool {
    MoveList moves;
    GenerateMoves(board, moves);
    bool found = false;
    for (const Move& m : moves) {
        if (m == move) found = true;
    }
    if (!found) return false;
    // We cannot set ourselves in check
    DoMove(move);
    auto valid = !IsInCheck(theBoard);
    UndoMove();
    return valid;
}
