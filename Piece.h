#pragma once

#include <iostream>
#include <numeric>

enum class Piece : uint8_t {
    NO_PIECE,
    WHITE_PAWN,
    WHITE_ROOK,
    WHITE_KNIGHT,
    WHITE_BISHOP,
    WHITE_QUEEN,
    WHITE_KING,
    BLACK_PAWN,
    BLACK_ROOK,
    BLACK_KNIGHT,
    BLACK_BISHOP,
    BLACK_QUEEN,
    BLACK_KING,
};

Piece InvertPiece(Piece piece);

inline std::ostream& operator<<(std::ostream& o, Piece piece) {
    switch (piece) {
    case Piece::NO_PIECE:
        o << ".";
        break;
    case Piece::WHITE_PAWN:
        o << "p";
        break;
    case Piece::WHITE_ROOK:
        o << "r";
        break;
    case Piece::WHITE_KNIGHT:
        o << "n";
        break;
    case Piece::WHITE_BISHOP:
        o << "b";
        break;
    case Piece::WHITE_QUEEN:
        o << "q";
        break;
    case Piece::WHITE_KING:
        o << "k";
        break;
    case Piece::BLACK_PAWN:
        o << "P";
        break;
    case Piece::BLACK_ROOK:
        o << "R";
        break;
    case Piece::BLACK_KNIGHT:
        o << "N";
        break;
    case Piece::BLACK_BISHOP:
        o << "B";
        break;
    case Piece::BLACK_QUEEN:
        o << "Q";
        break;
    case Piece::BLACK_KING:
        o << "K";
        break;
    }
    return o;
}