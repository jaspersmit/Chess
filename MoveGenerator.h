#pragma once

#include "Board.h"
#include "Move.h"
#include "MoveList.h"

void GenerateMoves(const Board& board, MoveList& moves, bool generateCastling = true);
auto IsInCheck(Board& board) -> bool;
auto IsMoveValid(const Board& board, const Move& move) -> bool;