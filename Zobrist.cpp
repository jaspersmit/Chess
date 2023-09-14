#include <iostream>
#include <random>

#include "Zobrist.h"

uint64_t piecePositionHashes[64 * 13];


uint64_t GetZobristHash(Square square, Piece piece) {
	return piecePositionHashes[static_cast<int>(piece) * 64 + square.rank * 8 + square.file];
}

int InitializePositionHashes() {
	std::mt19937_64 rng;
	int i;
	for (i = 0; i < 64 * 13; i++)
		piecePositionHashes[i] = rng();
	return i;
}

int numPiecePositionHashesInitialed = InitializePositionHashes();
