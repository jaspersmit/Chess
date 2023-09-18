#pragma once

#include <cstdint>

#include "Move.h"

enum class Bound {
	EXACT,
	LOWER_BOUND,
	UPPER_BOUND
};

struct TtEntry {
	uint64_t hash = 0;
	Bound bound;
	int depth = 0;
	int score = 0;
	Move bestMove = INVALID_MOVE;
};

auto GetEntry(uint64_t hash) -> TtEntry*;