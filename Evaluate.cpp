
#include <array>

#include "Evaluate.h"

int Pawns[] = {
	0,   0,   0,   0,   0,   0,   0,   0,
    50,  50,  50,  50,  50,  50,  50,  50,
    10,  10,  20,  30,  30,  20,  10,  10,
	5,   5,  10,  25,  25,  10,   5,   5,
	0,   0,   0,  20,  20,   0,   0,   0,
	5,  -5, -10,   0,   0, -10,  -5,   5,
	5,  10,  10, -20, -20,  10,  10,   5,
	0,   0,   0,   0,   0,   0,   0,   0
};

int Rooks[] = {
	0,  0,  0,  0,  0,  0,  0,  0,
	5, 10, 10, 10, 10, 10, 10,  5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	0,  0,  0,  5,  5,  0,  0,  0
};
int Knights[] = {
	-50,-40,-30,-30,-30,-30,-40,-50,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	-30,  0, 15, 20, 20, 15,  0,-30,
	-30,  5, 10, 15, 15, 10,  5,-30,
	-40,-20,  0,  5,  5,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50,
};
int Bishops[] = {
	-20,-10,-10,-10,-10,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5, 10, 10,  5,  0,-10,
	-10,  5,  5, 10, 10,  5,  5,-10,
	-10,  0, 10, 10, 10, 10,  0,-10,
	-10, 10, 10, 10, 10, 10, 10,-10,
	-10,  5,  0,  0,  0,  0,  5,-10,
	-20,-10,-10,-10,-10,-10,-10,-20,
};
int Queens[] = {
	-20,-10,-10, -5, -5,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5,  5,  5,  5,  0,-10,
	-5,  0,  5,  5,  5,  5,  0, -5,
	0,  0,  5,  5,  5,  5,  0, -5,
	-10,  5,  5,  5,  5,  5,  0,-10,
	-10,  0,  5,  0,  0,  0,  0,-10,
	-20,-10,-10, -5, -5,-10,-10,-20
};
int Kings[] = {
    -80, -70, -70, -70, -70, -70, -70, -80,
    -60, -60, -60, -60, -60, -60, -60, -60,
    -40, -50, -50, -60, -60, -50, -50, -40,
    -30, -40, -40, -50, -50, -40, -40, -30, 
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    20, 20, -5, -5, -5, -5, 20, 20, 20, 30,
    10, 0, 0, 10, 30, 20
};

std::array<int, 13 * 64> pieceLookup;

int CopyInvert(int index, int* sourceTable) {
    for (int i = 0; i < 64; i++) {
        int file = i % 8;
        int rank = i / 8;
        int sourceIndex = (7 - rank) * 8 + file;
        pieceLookup[index] = sourceTable[sourceIndex];
        index++;
    }
    return index;
}


int Copy(int index, int* sourceTable) {
    for (int i = 0; i < 64; i++) {
        pieceLookup[index] = -sourceTable[i];
        index++;
    }
    return index;
}

int CreatePieceLookup() {
    int index = 0;

    // NO_PIECE
    for (int i = 0; i < 64 * 13; i++) {
        pieceLookup[i] = 0;
    }

    index = 64;

    assert(static_cast<int>(Piece::WHITE_PAWN) == index / 64);
    index = CopyInvert(index, Pawns);

    assert(static_cast<int>(Piece::WHITE_ROOK) == index / 64);
    index = CopyInvert(index, Rooks);

    assert(static_cast<int>(Piece::WHITE_KNIGHT) == index / 64);
    index = CopyInvert(index, Knights);

    assert(static_cast<int>(Piece::WHITE_BISHOP) == index / 64);
    index = CopyInvert(index, Bishops);

    assert(static_cast<int>(Piece::WHITE_QUEEN) == index / 64);
    index = CopyInvert(index, Queens);

    assert(static_cast<int>(Piece::WHITE_KING) == index / 64);
    index = CopyInvert(index, Kings);

    assert(static_cast<int>(Piece::BLACK_PAWN) == index / 64);
    index = Copy(index, Pawns);

    assert(static_cast<int>(Piece::BLACK_ROOK) == index / 64);
    index = Copy(index, Rooks);

    assert(static_cast<int>(Piece::BLACK_KNIGHT) == index / 64);
    index = Copy(index, Knights);

    assert(static_cast<int>(Piece::BLACK_BISHOP) == index / 64);
    index = Copy(index, Bishops);

    assert(static_cast<int>(Piece::BLACK_QUEEN) == index / 64);
    index = Copy(index, Queens);

    assert(static_cast<int>(Piece::BLACK_KING) == index / 64);
    index = Copy(index, Kings);

    assert(index == 64 * 13);
    return index;
}

int pieceLookupInitialized = CreatePieceLookup();

#include <iostream>

int EvaluateBoard(const Board& board) {
    int score = 0;
    for (int8_t r = 7; r >= 0; r--) {
        for (int8_t f = 0; f < 8; f++) {
            switch (board({ r, f })) {
            case Piece::NO_PIECE:
                break;
            case Piece::WHITE_PAWN:
                score += 100;
                break;
            case Piece::WHITE_ROOK:
                score += 500;
                break;
            case Piece::WHITE_KNIGHT:
                score += 300;
                break;
            case Piece::WHITE_BISHOP:
                score += 320;
                break;
            case Piece::WHITE_QUEEN:
                score += 900;
                break;
            case Piece::WHITE_KING:
                score += 1000000;
                break;
            case Piece::BLACK_PAWN:
                score -= 100;
                break;
            case Piece::BLACK_ROOK:
                score -= 500;
                break;
            case Piece::BLACK_KNIGHT:
                score -= 300;
                break;
            case Piece::BLACK_BISHOP:
                score -= 320;
                break;
            case Piece::BLACK_QUEEN:
                score -= 900;
                break;
            case Piece::BLACK_KING:
                score -= 1000000;
                break;
            }
        }
    }

    for (int8_t rank = 0; rank < 8; rank++)
        for (int8_t file = 0; file < 8; file++) {
            Piece piece = board(Square{ rank, file });
            score += pieceLookup[64 * static_cast<int>(piece) + 8 * rank + file];
           // std::cout << "piece/square " << pieceLookup[64 * static_cast<int>(piece) + 8 * rank + file] << "\n";

        }

    return static_cast<int>(board.GetTurn()) * score;
}