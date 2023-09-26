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


auto QuiescenceSearch(Board& board, int depth, int alpha, int beta) -> int {
    auto entry = GetEntry(board.GetHash());
    auto hashMove = INVALID_MOVE;
    if (entry->hash == board.GetHash()) {
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

    std::vector<Move> moves;
    GenerateMoves(board, moves);
    std::vector<int> indices(moves.size());
    std::iota(indices.begin(), indices.end(), 0);

    OrderMoves(board, moves, indices, hashMove);

    auto maxScore = EvaluateBoard(board);
    auto bestMove = INVALID_MOVE;
    auto bound = Bound::UPPER_BOUND;

    for (const auto& index : indices) {
        auto& move = moves[index];
        // Search no further if king was captured
        if (board(move.to) == Piece::WHITE_KING ||
            board(move.to) == Piece::BLACK_KING)
            return MAX_SCORE;

        if (board.IsEmpty(move.to)) continue;

        Board nextBoard = board;
        nextBoard.ApplyMove(move);
        nextBoard.SwitchTurn();
        auto score = -QuiescenceSearch(nextBoard, depth - 1, -beta, -alpha);

        if (score > alpha) {
            bound = Bound::EXACT;
            alpha = score;
        }
        if (score >= beta) {
            entry->depth = depth;
            entry->hash = board.GetHash();
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
    entry->hash = board.GetHash();
    entry->bound = bound;
    entry->bestMove = bestMove;
    entry->score = maxScore;

    return maxScore;
}


auto MinMax(int depth, int alpha, int beta) -> int {
    if (depth == 0) {
        numEvaluates++;
        //return QuiescenceSearch(board, 0, alpha, beta);
        return EvaluateBoard(theBoard);
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

    std::vector<Move> moves;
    GenerateMoves(theBoard, moves);
    if (moves.size() == 0) {
        return 0;
    }


    std::vector<int> indices(moves.size());
    std::iota(indices.begin(), indices.end(), 0);

    OrderMoves(theBoard, moves, indices, hashMove);

    auto maxScore = -MAX_SCORE;
    auto bestMove = INVALID_MOVE;
    auto bound = Bound::UPPER_BOUND;

    for (const auto& index : indices) {
        auto& move = moves[index];
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
            bestMove = move;
            if (depth == searchDepth) {
                bestMoveSoFar = move;
            }
        }
    }

    if (maxScore == -MAX_SCORE) {
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
    entry->score = maxScore;

    return maxScore;
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
    while (searchRunning) {
        searchDepth++;
        MinMax(searchDepth, -1000000, 1000000);
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

// This might somewhat kill the hash?
auto IsInMate() -> bool {
    auto score = MinMax(2, -1000000, 1000000);
    return score == -MAX_SCORE;
}