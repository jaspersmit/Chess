#pragma once

#include "Square.h"

class Move {
public:
    Square from;
    Square to;

    auto operator==(const Move& other) const -> bool {
        return other.from == from && other.to == to;
    }
};

constexpr Move INVALID_MOVE = { {0,0}, {0,0} };
