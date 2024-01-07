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


struct Config {
    bool ComputerPlaysWhite = false;
    bool ComputerPlaysBlack = false;
};

void PlayLoop(Config config) {
    SetDefaultBoard(theBoard);
    std::cout << theBoard;
    while (true) {
        std::cout << theBoard;


        std::cout << EvaluateBoard(theBoard) << "\n";
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



//int main() {
//    //Benchmark();
//    Test();
//    PlayComputerVsHuman();
//    //PlayHumanVsComputer();
//    //PlayHumanVsHuman();
//
//}
