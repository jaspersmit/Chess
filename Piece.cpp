#include <cassert>

#include "Piece.h"

Piece InvertPiece(Piece piece) {
    //TODO Do bit magic
    switch (piece) {
    case Piece::NO_PIECE:
        return Piece::NO_PIECE;
    case Piece::MY_PAWN:
        return Piece::ENEMY_PAWN;
    case Piece::MY_ROOK:
        return Piece::ENEMY_ROOK;
    case Piece::MY_KNIGHT:
        return Piece::ENEMY_KNIGHT;
    case Piece::MY_BISHOP:
        return Piece::ENEMY_BISHOP;
    case Piece::MY_QUEEN:
        return Piece::ENEMY_QUEEN;
    case Piece::MY_KING:
        return Piece::ENEMY_KING;
    case Piece::ENEMY_PAWN:
        return Piece::MY_PAWN;
    case Piece::ENEMY_ROOK:
        return Piece::MY_ROOK;
    case Piece::ENEMY_KNIGHT:
        return Piece::MY_KNIGHT;
    case Piece::ENEMY_BISHOP:
        return Piece::MY_BISHOP;
    case Piece::ENEMY_QUEEN:
        return Piece::MY_QUEEN;
    case Piece::ENEMY_KING:
        return Piece::MY_KING;
    }
    assert(false);
    return Piece::NO_PIECE;

}