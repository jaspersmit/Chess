#pragma once

#include <vector>

#include "Board.h"
#include "Move.h"

void GenerateMoves(const Board& board, std::vector<Move>& moves, bool generateCastling = true);
auto IsInCheck(Board& board) -> bool;
auto IsMoveValid(const Board& board, const Move& move) -> bool;