#pragma once

#include <vector>

#include "Board.h"
#include "Move.h"
#include "MoveList.h"

constexpr int NUM_KILLERS = 2;
constexpr int MAX_KILLERS_DEPTH = 128;

struct Killers {
    int replace = 0;
    Move moves[NUM_KILLERS];

    auto Match(Move move) -> bool {
        for (int i = 0; i < NUM_KILLERS; i++) {
            if (moves[i] == move) return true;
        }
        return false;
    }

    void Add(Move move) {
        if (Match(move)) return;
        moves[replace] = move;
        replace = (replace + 1) % NUM_KILLERS;
    }
};

extern Killers killers[MAX_KILLERS_DEPTH];

void OrderMoves(const Board& board, MoveList& moves, std::array<int, 128>& indices, Move hashMove, Killers& killers);
