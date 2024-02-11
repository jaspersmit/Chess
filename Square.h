#pragma once

#include <numeric>

#include "Direction.h"

class Square {
public:
    int8_t rank = 0;
    int8_t file = 0;

    constexpr Square() {}

    constexpr Square(int8_t rank, int8_t file) :
        rank(rank), file(file) {

    }

    auto operator == (const Square& other) const -> bool {
        return other.file == file && other.rank == rank;
    }

    auto Add(int8_t r, int8_t f) const-> Square {
        return { static_cast<int8_t>(rank + r), static_cast<int8_t>(file + f) };
    }

    auto Add(Direction direction) const-> Square {
        return { static_cast<int8_t>(rank + direction.vertical), static_cast<int8_t>(file + direction.horizontal) };
    }

    auto IsValid() const -> bool {
        return rank >= 0 && rank < 8 && file >= 0 && file < 8;
    }
};

inline std::ostream& operator<<(std::ostream& o, const Square& square) {
    return o << static_cast<char>('A' + square.file) << (square.rank + 1);
}