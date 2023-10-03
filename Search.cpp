#include <iostream>
#include <thread>

#include "Evaluate.h"
#include "MoveGenerator.h"
#include "MoveOrder.h"
#include "Search.h"
#include "TranspositionTable.h"

int numEvaluates;
int numCacheHits;
int numCacheMisses;
int depthReached;

int searchDepth = 8;
volatile bool searchRunning = false;
auto bestMoveSoFar = INVALID_MOVE;

auto QuiescenceSearch(int depth, int alpha, int beta) -> int {
    auto entry = GetEntry(theBoard.GetHash());
    auto hashMove = INVALID_MOVE;
    if (entry->hash == theBoard.GetHash()) {
        hashMove = entry->bestMove;
        if (entry->depth >= depth) {
            numCacheHits++;
            if (entry->bound == Bound::EXACT) {
                return entry->score;
            }
            if (entry->bound == Bound::LOWER_BOUND && entry->score >= beta) {
                return beta;
            }
        }
    }
    else {
        numCacheMisses++;
    }

    MoveList moves;
    GenerateMoves(theBoard, moves);
    std::array<int, 128> indices;
    std::iota(indices.begin(), indices.begin() + moves.GetNumMoves(), 0);

    OrderMoves(theBoard, moves, indices, hashMove, depth < MAX_KILLERS_DEPTH ? killers[depth] : killers[0]);

    auto maxScore = EvaluateBoard(theBoard);
    auto bestMove = INVALID_MOVE;
    auto bound = Bound::UPPER_BOUND;

    for (int i = 0; i < moves.GetNumMoves(); i++) {
        auto index = indices[i];
        auto move = moves.GetMove(index);
        // Search no further if king was captured
        if (theBoard(move.to) == Piece::WHITE_KING ||
            theBoard(move.to) == Piece::BLACK_KING)
            return MAX_SCORE;

        if (theBoard.IsEmpty(move.to)) continue;

        DoMove(move);
        theBoard.SwitchTurn();
        auto score = -QuiescenceSearch(depth - 1, -beta, -alpha);
        theBoard.SwitchTurn();
        UndoMove();

        if (score > alpha) {
            bound = Bound::EXACT;
            alpha = score;
        }
        if (score >= beta) {
            entry->depth = depth;
            entry->hash = theBoard.GetHash();
            entry->bound = Bound::LOWER_BOUND;
            entry->bestMove = move;
            entry->score = score;
            return beta;
        }
        if (score > maxScore) {
            maxScore = score;
        }
    }

    entry->depth = depth;
    entry->hash = theBoard.GetHash();
    entry->bound = bound;
    entry->bestMove = bestMove;
    entry->score = maxScore;

    return maxScore;
}


auto MinMax(int depth, int alpha, int beta) -> int {
    if (depth == 0) {
        numEvaluates++;
        return QuiescenceSearch(0, alpha, beta);
        //return EvaluateBoard(theBoard);
    }

    auto entry = GetEntry(theBoard.GetHash());
    auto hashMove = INVALID_MOVE;
    if (entry->hash == theBoard.GetHash()) {
        hashMove = entry->bestMove;
        if (entry->depth >= depth) {
            numCacheHits++;
            if (entry->bound == Bound::EXACT) {
                return entry->score;
            }
            if (entry->bound == Bound::LOWER_BOUND && entry->score >= beta) {
                return beta;
            }
        }
    }
    else {
        numCacheMisses++;
    }

    MoveList moves;
    GenerateMoves(theBoard, moves);
    if (moves.GetNumMoves() == 0) {
        return 0;
    }

    std::array<int, 128> indices;
    std::iota(indices.begin(), indices.end(), 0);

    OrderMoves(theBoard, moves, indices, hashMove, depth < MAX_KILLERS_DEPTH ? killers[depth] : killers[0]);

    auto bestMove = INVALID_MOVE;
    auto bound = Bound::UPPER_BOUND;

    for (int i = 0; i < moves.GetNumMoves(); i++) {
        auto index = indices[i];
        auto move = moves.GetMove(index);
        // Search no further if king was captured
        if (theBoard(move.to) == Piece::WHITE_KING ||
            theBoard(move.to) == Piece::BLACK_KING)
            return MAX_SCORE;

        DoMove(move);
        theBoard.SwitchTurn();
        auto score = -MinMax(depth - 1, -beta, -alpha);
        theBoard.SwitchTurn();
        UndoMove();

        if (!searchRunning) {
            // Search aborted, return value is garbage when search is aborted
            return 0;
        }

        //Adjust score for number of moves
        if (score > MAX_SCORE - 128) score--;

        if (depth == searchDepth) {
            std::cout << depth << " " << move << " " << score << "\n";
        }

        if (score >= beta) {
            if (theBoard(move.to) == Piece::NO_PIECE && depth < MAX_KILLERS_DEPTH) {
                killers[depth].Add(move);
            }

            entry->depth = depth;
            entry->hash = theBoard.GetHash();
            entry->bound = Bound::LOWER_BOUND;
            entry->bestMove = move;
            entry->score = score;
            return beta;
        }
        if (score > alpha) {
            bound = Bound::EXACT;
            alpha = score;
            bestMove = move;
            if (depth == searchDepth) {
                bestMoveSoFar = move;
            }
        }
    }

    if (alpha == -MAX_SCORE) {
        // The enemy can take my king next turn
        // If he can take it now, then we are in mate
        theBoard.SwitchTurn();
        auto s = -MinMax(1, -beta, -alpha);
        theBoard.SwitchTurn();
        if (s == -MAX_SCORE) {
            return -MAX_SCORE;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               return -MAX_SCORE;
        }
        else {
            // Otherwise it is stale mate
            return 0;
        }
    }

    entry->depth = depth;
    entry->hash = theBoard.GetHash();
    entry->bound = bound;
    entry->bestMove = bestMove;
    entry->score = alpha;

    return alpha;
}

auto FindBestMove() -> Move {
    numEvaluates = 0;
    numCacheHits = 0;
    numCacheMisses = 0;
    MinMax(searchDepth, -1000000, 1000000);
    auto entry = GetEntry(theBoard.GetHash());
    assert(entry->hash == theBoard.GetHash());
    return entry->bestMove;
}

auto SearchInThread() {
    searchDepth = 1;
    auto score = 0;
    while (searchRunning) {
        searchDepth++;

        auto delta = 100;
        auto alpha = score - delta;
        auto beta = score + delta;

        while (true) {
            score = MinMax(searchDepth, alpha, beta);
            if (score <= alpha) {
                alpha -= delta;
                delta += delta / 3;
            }
            else if (score >= beta) {
                beta += delta;
                delta += delta / 3;
            }
            else {
                break;
            }
        }
    }
    depthReached = searchDepth;
}

auto FindBestMoveInTime() -> Move {
    numEvaluates = 0;
    numCacheHits = 0;
    numCacheMisses = 0;
    depthReached = 1;
    searchRunning = true;
    std::thread t([&]() { SearchInThread(); });
    std::this_thread::sleep_for(std::chrono::seconds(10));
    searchRunning = false;
    t.join();
    return bestMoveSoFar;
}

auto IsInMate() -> bool {
    searchDepth = 4; // To prevent print
    searchRunning = true;
    auto score = MinMax(2, -1000000, 1000000);
    searchRunning = false;
    return score == -MAX_SCORE;
}

void Benchmark() {
    ParseBoard(theBoard,
        "RNBQKBNR"
        "PPPPPPPP"
        "........"
        "........"
        "........"
        "........"
        "pppppppp"
        "rnbqkbnr"
    );
    auto startTime = std::chrono::high_resolution_clock::now();
    searchDepth = 10;
    searchRunning = true;

    auto alpha = -100;
    auto beta = 100;
    auto delta = 100;
    while (true) {

        auto result = MinMax(searchDepth, alpha, beta);
        if (result <= alpha) {
            alpha -= delta;
            delta += delta / 3;
        }
        else if (result >= beta) {
            beta += delta;
            delta += delta / 3;
        }
        else {
            break;
        }
    }

    searchRunning = false;
    auto endTime = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    std::cout << "Time taken " << ms << " ms";
}