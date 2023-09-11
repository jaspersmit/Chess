#pragma once

#include <cassert>

#include "Square.h"
#include "Piece.h"

constexpr int MAX_SCORE = 1000000;


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

    auto HasFriend(Square square) const -> bool {
        return (*this)(square) >= Piece::MY_PAWN && (*this)(square) < Piece::ENEMY_PAWN;
    }

    auto HasEnemy(Square square) const -> bool {
        return (*this)(square) >= Piece::ENEMY_PAWN;
    }

    void Invert() {
        for (int i1 = 0; i1 < 32; i1++) {
            auto i2 = 63 - i1;
            auto p1 = InvertPiece(pieces[i1]);
            auto p2 = InvertPiece(pieces[i2]);
            pieces[i2] = p1;
            pieces[i1] = p2;
        }
    }

private:
    Piece pieces[64];

};