#include <iostream>
#include <random>

#include "Zobrist.h"

uint64_t piecePositionHashes[64 * 13];
uint64_t turnHash;
uint64_t castlingRightsHashes[2][2];
uint64_t enPassantHashes[8];


uint64_t GetZobristHash(Square square, Piece piece) {
	return piecePositionHashes[static_cast<int>(piece) * 64 + square.rank * 8 + square.file];
}

bool InitializePositionHashes() {
	std::mt19937_64 randomNumberGenerator;
	turnHash = randomNumberGenerator();

	for (auto color = 0; color < 2; color++) {
		for (auto side = 0; side < 2; side++) {
			castlingRightsHashes[color][side] = randomNumberGenerator();
		}
	}
	for (int i = 0; i < 8; i++)
		enPassantHashes[i] = randomNumberGenerator();

	for (auto i = 0; i < 64 * 13; i++)
		piecePositionHashes[i] = randomNumberGenerator();
	return true;
}

int numPiecePositionHashesInitialized = InitializePositionHashes();
