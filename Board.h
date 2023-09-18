#pragma once

#include <iostream>
#include <cassert>

#include "Square.h"
#include "Move.h"
#include "Piece.h"
#include "Zobrist.h"

constexpr int MAX_SCORE = 1000000;

enum class Color : int8_t {
    WHITE = 1,
    NEUTRAL = 0,
    BLACK = -1
};

inline std::ostream& operator <<(std::ostream& o, Color color) {
    switch (color) {
    case Color::WHITE:
        o << "WHITE";
        break;
    case Color::NEUTRAL:
        o << "NEUTRAL";
        break;
    case Color::BLACK:
        o << "BLACK";
        break;
    }
    return o;
}


constexpr Color InvertColor(Color color) {
    return static_cast<Color>(-static_cast<int8_t>(color));
}


class Board {
public:
    auto operator() (Square square) -> Piece& {
        assert(square.rank * 8 + square.file < 64);
        assert(square.rank * 8 + square.file >= 0);
        return pieces[square.rank * 8 + square.file];
    }

    auto operator() (Square square) const -> const Piece& {
        assert(square.rank * 8 + square.file < 64);
        assert(square.rank * 8 + square.file >= 0);
        return pieces[square.rank * 8 + square.file];
    }

    auto IsEmpty(Square square) const -> bool {
        return (*this)(square) == Piece::NO_PIECE;
    }

    auto IsWhite(Square square) const -> bool {
        return (*this)(square) >= Piece::WHITE_PAWN && (*this)(square) < Piece::BLACK_PAWN;
    }

    auto IsCurrentPlayer(Square square) const -> bool {
        return GetColor(square) == GetTurn();
    }

    auto IsOtherPlayer(Square square) const -> bool {
        return GetColor(square) == InvertColor(GetTurn());
    }

    auto GetColor(Square square) const -> Color {
        auto piece = (*this)(square);
        if (piece == Piece::NO_PIECE) return Color::NEUTRAL;
        else if (piece < Piece::BLACK_PAWN) return Color::WHITE;
        else return Color::BLACK;
    }

    auto IsBlack(Square square) const -> bool {
        return (*this)(square) >= Piece::BLACK_PAWN;
    }

    auto GetTurn() const -> Color {
        return turn;
    }

    auto GetHash() const -> uint64_t {
        return hash;
    }

    inline void SetSquare(Square square, Piece piece) {
        auto existingPiece = (*this)(square);
        if (piece == existingPiece) return;
        if (existingPiece != Piece::NO_PIECE) {
            hash ^= GetZobristHash(square, existingPiece);
        }
        if (piece != Piece::NO_PIECE) {
            hash ^= GetZobristHash(square, piece);
        }
        (*this)(square) = piece;
    }

    void ApplyMove(const Move& move);

    void SwitchTurn() {
        turn = InvertColor(turn);
        hash ^= turnHash;
    }

private:
    Piece pieces[64] = {};
    Color turn = Color::WHITE;
    uint64_t hash = 0;
};