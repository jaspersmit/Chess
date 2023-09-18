#pragma once

#include <numeric>

#include "Direction.h"

class Square {
public:
    int8_t rank;
    int8_t file;

    constexpr Square(int8_t rank, int8_t file) :
        rank(rank), file(file) {

    }

    auto operator+ (const Square& other) const -> Square {
        return { rank + other.rank, file + other.file };
    }

    auto operator == (const Square& other) const -> bool {
        return other.file == file && other.rank == rank;
    }

    auto Add(int8_t r, int8_t f) const-> Square {
        return { rank + r, file + f };
    }

    auto Add(Direction direction) const-> Square {
        return { rank + direction.vertical, file + direction.horizontal };
    }

    auto IsValid() const -> bool {
        return rank >= 0 && rank < 8 && file >= 0 && file < 8;
    }
};
