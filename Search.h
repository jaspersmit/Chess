#pragma once

#include "Board.h"
#include "Move.h"

extern int depthReached;
extern int numEvaluates;
extern int numCacheHits;
extern int numCacheMisses;

auto MinMax(int depth, int alpha, int beta)->int;
auto FindBestMove()->Move;
auto FindBestMoveInTime() -> Move;
auto IsInMate() -> bool;
void Benchmark();