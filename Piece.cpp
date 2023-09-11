#include <cassert>

#include "Piece.h"

Piece InvertPiece(Piece piece) {
    //TODO Do bit magic
    switch (piece) {
    case Piece::NO_PIECE:
        return Piece::NO_PIECE;
    case Piece::WHITE_PAWN:
        return Piece::BLACK_PAWN;
    case Piece::WHITE_ROOK:
        return Piece::BLACK_ROOK;
    case Piece::WHITE_KNIGHT:
        return Piece::BLACK_KNIGHT;
    case Piece::WHITE_BISHOP:
        return Piece::BLACK_BISHOP;
    case Piece::WHITE_QUEEN:
        return Piece::BLACK_QUEEN;
    case Piece::WHITE_KING:
        return Piece::BLACK_KING;
    case Piece::BLACK_PAWN:
        return Piece::WHITE_PAWN;
    case Piece::BLACK_ROOK:
        return Piece::WHITE_ROOK;
    case Piece::BLACK_KNIGHT:
        return Piece::WHITE_KNIGHT;
    case Piece::BLACK_BISHOP:
        return Piece::WHITE_BISHOP;
    case Piece::BLACK_QUEEN:
        return Piece::WHITE_QUEEN;
    case Piece::BLACK_KING:
        return Piece::WHITE_KING;
    }
    assert(false);
    return Piece::NO_PIECE;

}