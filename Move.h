#pragma once

#include <ostream>

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

inline std::ostream& operator<<(std::ostream& o, const Move& move) {
    return o << move.from << " -> " << move.to;
}

auto ParseMove(const std::string& moveString) -> Move;