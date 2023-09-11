#pragma once

#include <vector>

#include "Board.h"
#include "Move.h"

void OrderMoves(const Board& board, std::vector<Move>& moves, std::vector<int>& indices);
