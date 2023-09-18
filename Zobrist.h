#pragma once


#include "Square.h"
#include "Piece.h"

extern uint64_t turnHash;

uint64_t GetZobristHash(Square square, Piece piece);
