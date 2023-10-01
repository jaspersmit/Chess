#pragma once


#include "Square.h"
#include "Piece.h"

extern uint64_t turnHash;
extern uint64_t castlingRightsHashes[2][2];
extern uint64_t enPassantHashes[8];

uint64_t GetZobristHash(Square square, Piece piece);
