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

struct Config {
    bool ComputerPlaysWhite = false;
    bool ComputerPlaysBlack = false;
};

void PlayLoop(Config config) {
    SetDefaultBoard(theBoard);

    std::cout << theBoard;
    while (true) {
        std::cout << theBoard;

        auto turn = theBoard.GetTurn();
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
                if (!IsMoveValid(theBoard, move)) {
                    std::cout << "Invalid move\n";
                    move = INVALID_MOVE;
                }
            }
            std::cout << "Your move " << move << "\n";
            DoMove(move);
        }
        else {
            auto move = FindBestMoveInTime();
            std::cout << "Depth reached " << depthReached << "\n";
            std::cout << "Evaluated " << numEvaluates << " nodes\n";
            std::cout << "Cache hits " << numCacheHits << ", misses " << numCacheMisses << "\n";
            DoMove(move);
            std::cout << "Computer played " << move << "\n";
        }

        theBoard.SwitchTurn();
        if (IsInMate()) {
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

void Test();

int main() {
    //Benchmark();
    Test();
    PlayHumanVsComputer();
}
