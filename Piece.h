#pragma once

#include <numeric>

enum class Piece : uint8_t {
    NO_PIECE,
    MY_PAWN,
    MY_ROOK,
    MY_KNIGHT,
    MY_BISHOP,
    MY_QUEEN,
    MY_KING,
    ENEMY_PAWN,
    ENEMY_ROOK,
    ENEMY_KNIGHT,
    ENEMY_BISHOP,
    ENEMY_QUEEN,
    ENEMY_KING,
};

Piece InvertPiece(Piece piece);