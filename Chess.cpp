#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include <cassert>

#include "Board.h"
#include "Evaluate.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "MoveOrder.h"
#include "Piece.h"
#include "Search.h"
#include "TranspositionTable.h"
#include "Zobrist.h"


void SetDefaultBoard(Board& board) {
    ParseBoard(board,
        "RNBQKBNR"
        "PPPPPPPP"
        "........"
        "........"
        "........"
        "........"
        "pppppppp"
        "rnbqkbnr"
    );


    ParseBoard(board,
        "........"
        "........"
        "........"
        "...K...."
        "..R....."
        "...k...."
        "........"
        "........"
    );
}

std::ostream& operator<<(std::ostream& o, Piece piece) {
    switch (piece) {
    case Piece::NO_PIECE:
        o << ".";
        break;
    case Piece::WHITE_PAWN:
        o << "p";
        break;
    case Piece::WHITE_ROOK:
        o << "r";
        break;
    case Piece::WHITE_KNIGHT:
        o << "n";
        break;
    case Piece::WHITE_BISHOP:
        o << "b";
        break;
    case Piece::WHITE_QUEEN:
        o << "q";
        break;
    case Piece::WHITE_KING:
        o << "k";
        break;
    case Piece::BLACK_PAWN:
        o << "P";
        break;
    case Piece::BLACK_ROOK:
        o << "R";
        break;
    case Piece::BLACK_KNIGHT:
        o << "N";
        break;
    case Piece::BLACK_BISHOP:
        o << "B";
        break;
    case Piece::BLACK_QUEEN:
        o << "Q";
        break;
    case Piece::BLACK_KING:
        o << "K";
        break;
    }
    return o;
}

std::ostream& operator<<(std::ostream& o, const Board& board) {
    for (int8_t r = 7; r >= 0; r--) {
        for (int8_t f = 0; f < 8; f++) {
            o << board({ r, f });
        }
        o << "\n";
    }
    return o;
}

auto ParseFile(char f) -> int8_t {
    if (f < 'A' || f > 'H') return -1;
    return f - 'A';
}

auto ParseRank(char f) -> int8_t {
    if (f < '1' || f > '8') return -1;
    return f - '1';
}

auto ParseMove(const std::string& moveString) -> Move {
    if (moveString.length() < 4) return INVALID_MOVE;
    auto f1 = ParseFile(moveString[0]);
    auto r1 = ParseRank(moveString[1]);
    auto f2 = ParseFile(moveString[2]);
    auto r2 = ParseRank(moveString[3]);
    if (f1 == -1 || r1 == -1 || f2 == -1 || r2 == -1) return INVALID_MOVE;
    return { {r1,f1}, {r2, f2} };
}

auto IsMoveValid(const Board& board, const Move& move) -> bool {
    std::vector<Move> moves;
    GenerateMoves(board, moves);
    bool found = false;
    for (const Move& m : moves) {
        if (m == move) found = true;
    }
    if (!found) return false;
    // We cannot set ourselves in check
    auto testBoard = board;
    testBoard.ApplyMove(move);
    return !IsInCheck(testBoard);
}



struct Config {
    bool ComputerPlaysWhite = false;
    bool ComputerPlaysBlack = false;
};

void PlayLoop(Config config) {
    Board board;
    SetDefaultBoard(board);

    std::cout << board;
    while (true) {
        std::cout << board;

        auto turn = board.GetTurn();
        auto playComputer =
            (turn == Color::WHITE && config.ComputerPlaysWhite) ||
            (turn == Color::BLACK && config.ComputerPlaysBlack);

        if (!playComputer) {
            auto move = INVALID_MOVE;
            while (move == INVALID_MOVE) {
                std::cout << "Your move: ";
                std::string moveString;
                std::cin >> moveString;
                move = ParseMove(moveString);
                if (!IsMoveValid(board, move)) {
                    std::cout << "Invalid move\n";
                    move = INVALID_MOVE;
                }
            }
            std::cout << "Your move " << move << "\n";
            board.ApplyMove(move);
        }
        else {
            auto move = FindBestMove(board);
            std::cout << "Evaluated " << numEvaluates << " nodes\n";
            std::cout << "Cache hits " << numCacheHits << ", misses " << numCacheMisses << "\n";
            board.ApplyMove(move);
            std::cout << "Computer played " << move << "\n";
        }

        board.SwitchTurn();
        if (IsInMate(board)) {
            std::cout << "Mate\n";
            return;
        }
    }
}

void PlayComputerVsComputer() {
    PlayLoop({ true, true });
}

void PlayHumanVsHuman() {
    PlayLoop({ false, false });
}

void PlayHumanVsComputer() {
    PlayLoop({ false, true });
}

void PlayComputerVsHuman() {
    PlayLoop({ true, false });
}

int main() {
    PlayHumanVsComputer();
}
