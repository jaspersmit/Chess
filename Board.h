#pragma once

#include <iostream>
#include <cassert>

#include "Square.h"
#include "Move.h"
#include "Piece.h"
#include "Zobrist.h"

constexpr int MAX_SCORE = 1000000;
constexpr int8_t INVALID_ENPASSENT_FILE = 8;

enum class Color : int8_t {
    WHITE = 1,
    NEUTRAL = 0,
    BLACK = -1
};

enum class CastlingSide : int8_t {
    QUEEN = 0,
    KING = 1
};

constexpr int ColorToIndex(Color color) {
    switch (color) {
    case Color::WHITE: return 0;
    case Color::BLACK: return 1;
    }
    assert(false);
}

constexpr Color GetColorOfPiece(Piece piece) {
    if (piece == Piece::NO_PIECE) return Color::NEUTRAL;
    else if (piece <= Piece::WHITE_KING) return Color::WHITE;
    else return Color::BLACK;
}

constexpr int CastlingSideToIndex(CastlingSide side) {
    return static_cast<int>(side);
}

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

    inline void Reset() {
        for (int i = 0; i < 64; i++) {
            pieces[i] = Piece::NO_PIECE;
        }
        hash = 0;
        for(auto i = 0; i < 2; i++) 
            for(auto j = 0; j < 2; j++)
                castlingRights[i][j] = true;
        turn = Color::WHITE;
        enPassantFile = INVALID_ENPASSENT_FILE;
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

    inline void SwitchTurn() {
        turn = InvertColor(turn);
        hash ^= turnHash;
    }

    inline auto HasCastlingRights(CastlingSide side) const -> bool {
        return castlingRights[ColorToIndex(turn)][CastlingSideToIndex(side)];
    }

    inline void SetCastlingRights(CastlingSide side, bool rights) {
        if (HasCastlingRights(side) != rights) {
            hash ^= castlingRightsHashes[ColorToIndex(turn)][CastlingSideToIndex(side)];
            castlingRights[ColorToIndex(turn)][CastlingSideToIndex(side)] = rights;
        }
    }

    void SetEnPassentFile(int8_t newEnpassantFile) {
        if (enPassantFile != INVALID_ENPASSENT_FILE)
            hash ^= enPassantHashes[enPassantFile];
        if (newEnpassantFile != INVALID_ENPASSENT_FILE)
            hash ^= enPassantHashes[newEnpassantFile];
        enPassantFile = newEnpassantFile;
    }

    auto GetEnPassentFile() const -> uint8_t {
        return enPassantFile;
    }

private:
    Piece pieces[64] = {};
    Color turn = Color::WHITE;
    uint64_t hash = 0;
    bool castlingRights[2][2];
    int8_t enPassantFile = 8;
};

inline std::ostream& operator<<(std::ostream& o, const Board& board) {
    for (int8_t r = 7; r >= 0; r--) {
        for (int8_t f = 0; f < 8; f++) {
            o << board({ r, f });
        }
        o << "\n";
    }
    return o;
}

extern Board theBoard;

void DoMove(const Move& move);
void UndoMove();
void ParseBoard(Board& board, const std::string& str);
void SetDefaultBoard(Board& board);
std::string GetProtocolString(const Board& board);