#pragma once

#include "Board.h"
#include "Move.h"

extern int depthReached;
extern int numEvaluates;
extern int numCacheHits;
extern int numCacheMisses;

auto MinMax(Board& board, int depth, int alpha, int beta)->int;
auto FindBestMove(Board& board)->Move;
auto FindBestMoveInTime(Board& board) -> Move;
auto IsInMate(Board& board) -> bool;