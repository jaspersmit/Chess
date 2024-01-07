#include <sstream>

#include "Move.h"

auto ParseFile(char f) -> int8_t {
    if (f >= 'A' && f <= 'H') return f - 'A';
    if (f >= 'a' && f <= 'h') return f - 'a';
    return -1;
}

auto ParseRank(char f) -> int8_t {
    if (f < '1' || f > '8') return -1;
    return f - '1';
}

auto ParseMove(const std::string& moveString) -> Move {
    if (moveString.length() < 4) return INVALID_MOVE;
    auto f1 = ParseFile(moveString[0]);
    auto r1 = ParseRank(moveString[1]);
    auto f2 = ParseFile(moveString[2]);
    auto r2 = ParseRank(moveString[3]);
    if (f1 == -1 || r1 == -1 || f2 == -1 || r2 == -1) return INVALID_MOVE;
    return { {r1,f1}, {r2, f2} };
}

std::string MoveToUCI(const Move& move) {
    std::stringstream ss;
    ss << static_cast<char>('a' + move.from.file)
        << (move.from.rank + 1)
        << static_cast<char>('a' + move.to.file)
        << (move.to.rank + 1);
    return ss.str();
}
