#pragma once

#include <array>

#include "Move.h"

class MoveList {
public:
	typedef Move* iterator;
	typedef const Move* const_iterator;

	auto GetNumMoves() const -> int {
		return numMoves;
	}

	auto GetMove(int index) const -> Move {
		return moves[index];
	}

	auto AddMove(Move move) {
		moves[numMoves++] = move;
	}

	auto begin() -> iterator {
		return &moves[0];
	}

	auto end() -> iterator {
		return &moves[numMoves];
	}

private:
	std::array<Move, 128> moves;
	int numMoves = 0;
};