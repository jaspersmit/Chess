#pragma once

#include <optional>

#include "Board.h"
#include "Move.h"

void ReadBook();
std::optional<Move> GetBookMove(const Board& board);
