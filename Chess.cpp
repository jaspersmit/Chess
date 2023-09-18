#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include <cassert>

#include "Board.h"
#include "Evaluate.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "MoveOrder.h"
#include "Piece.h"
#include "TranspositionTable.h"
#include "Zobrist.h"



void SetDefaultBoard(Board& board) {
    for (int8_t r = 0; r < 8; r++)
        for (int8_t f = 0; f < 8; f++)
            board({ r, f }) = Piece::NO_PIECE;
    
    for (int8_t f = 0; f < 8; f++) {
        board({ 1, f }) = Piece::WHITE_PAWN;
        board({ 6, f }) = Piece::BLACK_PAWN;
    }

    board({ 0, 0 }) = Piece::WHITE_ROOK;
    board({ 0, 1 }) = Piece::WHITE_KNIGHT;
    board({ 0, 2 }) = Piece::WHITE_BISHOP;
    board({ 0, 3 }) = Piece::WHITE_QUEEN;
    board({ 0, 4 }) = Piece::WHITE_KING;
    board({ 0, 5 }) = Piece::WHITE_BISHOP;
    board({ 0, 6 }) = Piece::WHITE_KNIGHT;
    board({ 0, 7 }) = Piece::WHITE_ROOK;
    board({ 7, 0 }) = Piece::BLACK_ROOK;
    board({ 7, 1 }) = Piece::BLACK_KNIGHT;
    board({ 7, 2 }) = Piece::BLACK_BISHOP;
    board({ 7, 3 }) = Piece::BLACK_QUEEN;
    board({ 7, 4 }) = Piece::BLACK_KING;
    board({ 7, 5 }) = Piece::BLACK_BISHOP;
    board({ 7, 6 }) = Piece::BLACK_KNIGHT;
    board({ 7, 7 }) = Piece::BLACK_ROOK;
}

std::ostream& operator<<(std::ostream& o, Piece piece) {
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

std::ostream& operator<<(std::ostream& o, const Board& board) {
    for (int8_t r = 7; r >= 0; r--) {
        for (int8_t f = 0; f < 8; f++) {
            o << board({ r, f });
        }
        o << "\n";
    }
    return o;
}

std::ostream& operator<<(std::ostream& o, const Square& square) {
    return o << static_cast<char>('A' + square.file) << (square.rank + 1);
}

std::ostream& operator<<(std::ostream& o, const Move& move) {
    return o << move.from << " -> " << move.to;
}


int numEvaluates;
int numCacheHits;
int numCacheMisses;

int searchDepth = 6;


int QuiescenceSearch(Board& board, int depth, int alpha, int beta) {
    std::vector<Move> moves;
    GenerateMoves(board, moves);

    auto entry = GetEntry(board.GetHash());
    auto hashMove = INVALID_MOVE;
    if (entry->hash == board.GetHash() && entry->depth >= depth) {
        numCacheHits++;
        hashMove = entry->bestMove;
        if (entry->bound == Bound::EXACT) {
            return entry->score;
        }
        if (entry->bound == Bound::LOWER_BOUND && entry->score >= beta) {
            return beta;
        }
    }
    else {
        numCacheMisses++;
    }

    std::vector<int> indices(moves.size());
    std::iota(indices.begin(), indices.end(), 0);

    OrderMoves(board, moves, indices, hashMove);

    auto maxScore = EvaluateBoard(board);
    auto bestMove = INVALID_MOVE;
    auto bound = Bound::UPPER_BOUND;

    for (const auto& index : indices) {
        auto& move = moves[index];
        // Search no further if king was captured
        if (board(move.to) == Piece::BLACK_KING)
            return MAX_SCORE;

        if (board.IsEmpty(move.to)) continue;

        Board nextBoard = board;
        nextBoard.ApplyMove(move);
        nextBoard.SwitchTurn();
        auto score = -QuiescenceSearch(nextBoard, depth - 1, -beta, -alpha);

        if (score > alpha) {
            bound = Bound::EXACT;
            alpha = score;
        }
        if (score >= beta) {
            entry->depth = depth;
            entry->hash = board.GetHash();
            entry->bound = Bound::LOWER_BOUND;
            entry->bestMove = move;
            entry->score = score;
            return beta;
        }
        if (score > maxScore) {
            maxScore = score;
        }
    }

    entry->depth = depth;
    entry->hash = board.GetHash();
    entry->bound = bound;
    entry->bestMove = bestMove;
    entry->score = maxScore;

    return maxScore;
}


int MinMax(Board& board, int depth, int alpha, int beta) {
    if (depth == 0) {
        numEvaluates++;
        return QuiescenceSearch(board, 0, alpha, beta);
    }

    auto entry = GetEntry(board.GetHash());
    auto hashMove = INVALID_MOVE;
    if (entry->hash == board.GetHash() && entry->depth >= depth) {
        numCacheHits++;
        hashMove = entry->bestMove;
        if (entry->bound == Bound::EXACT) {
            return entry->score;
        }
        if (entry->bound == Bound::LOWER_BOUND && entry->score >= beta) {
            return beta;
        }
    }
    else {
        numCacheMisses++;
    }

    std::vector<Move> moves;
    GenerateMoves(board, moves);

    std::vector<int> indices(moves.size());
    std::iota(indices.begin(), indices.end(), 0);

    OrderMoves(board, moves, indices, hashMove);

    auto maxScore = -MAX_SCORE;
    auto bestMove = INVALID_MOVE;
    auto bound = Bound::UPPER_BOUND;

    for (const auto& index : indices) {
        auto& move = moves[index];
        // Search no further if king was captured
        if (board(move.to) == Piece::BLACK_KING) 
            return MAX_SCORE;

        Board nextBoard = board;
        nextBoard.ApplyMove(move);
        nextBoard.SwitchTurn();
        auto score = -MinMax(nextBoard, depth - 1, -beta, -alpha);

        if (depth == searchDepth) {
            std::cout << depth << " " << move << " " << score << "\n";
        }

        if (score > alpha) {
            bound = Bound::EXACT;
            alpha = score;
        }
        if (score >= beta) {
            entry->depth = depth;
            entry->hash = board.GetHash();
            entry->bound = Bound::LOWER_BOUND;
            entry->bestMove = move;
            entry->score = score;
            return beta;
        }
        if (score > maxScore) {
            maxScore = score;
            bestMove = move;
        }
    }

    entry->depth = depth;
    entry->hash = board.GetHash();
    entry->bound = bound;
    entry->bestMove = bestMove;
    entry->score = maxScore;

    return maxScore;
}

Move FindBestMove(Board& board, int depth) {
    numEvaluates = 0;
    numCacheHits = 0;
    numCacheMisses = 0;

    MinMax(board, depth, -1000000, 1000000);
    auto entry = GetEntry(board.GetHash());
    assert(entry->hash == board.GetHash());
    return entry->bestMove;
}

auto ParseFile(char f) -> int8_t {
    if (f < 'A' || f > 'H') return -1;
    return f - 'A';
}

auto ParseRank(char f) -> int8_t {
    if (f < '1' || f > '8') return -1;
    return f - '1';
}

auto ParseMove(const std::string& moveString) -> Move {
    if (moveString.length() < 4) return INVALID_MOVE;
    auto f1 = ParseFile(moveString[0]);
    auto r1 = ParseRank(moveString[1]);
    auto f2 = ParseFile(moveString[2]);
    auto r2 = ParseRank(moveString[3]);
    if (f1 == -1 || r1 == -1 || f2 == -1 || r2 == -1) return INVALID_MOVE;
    return { {r1,f1}, {r2, f2} };
}

auto IsMoveValid(const Board& board, const Move& move) -> bool {
    std::vector<Move> moves;
    GenerateMoves(board, moves);
    for (const Move& m : moves) {
        if (m == move) return true;
    }
    return false;
}

void ParseBoard(Board& board, const std::string& str) {
    if (str.length() != 64) {
        std::cout << "Could not parse board";
    }

    for (int i = 0; i < 64; i++) {
        char c = str[i];
        auto piece = Piece::NO_PIECE;

        switch (c) {
        case 'p':
            piece = Piece::WHITE_PAWN;
            break;
        case 'r':
            piece = Piece::WHITE_ROOK;
            break;
        case 'n':
            piece = Piece::WHITE_KNIGHT;
            break;
        case 'b':
            piece = Piece::WHITE_BISHOP;
            break;
        case 'k':
            piece = Piece::WHITE_KING;
            break;
        case 'q':
            piece = Piece::WHITE_QUEEN;
            break;
        case 'P':
            piece = Piece::BLACK_PAWN;
            break;
        case 'R':
            piece = Piece::BLACK_ROOK;
            break;
        case 'N':
            piece = Piece::BLACK_KNIGHT;
            break;
        case 'B':
            piece = Piece::BLACK_BISHOP;
            break;
        case 'K':
            piece = Piece::BLACK_KING;
            break;
        case 'Q':
            piece = Piece::BLACK_QUEEN;
            break;
        }
        int8_t rank = 7 - (i / 8);
        int8_t file = i % 8;
        board({ rank, file }) = piece;
    }
}


void PlayLoop() {
    Board board;
    SetDefaultBoard(board);

    std::cout << board;
    while (true) {
        auto move = INVALID_MOVE;
        while (move == INVALID_MOVE) {
            std::cout << "Your move: ";
            std::string moveString;
            std::cin >> moveString;
            move = ParseMove(moveString);
            if (!IsMoveValid(board, move)) {
                std::cout << "Invalid move\n";
                move = INVALID_MOVE;
            }
        }
        std::cout << "Your move " << move << "\n";
        board.ApplyMove(move);
        board.SwitchTurn();

        std::cout << board;


        move = FindBestMove(board, searchDepth);
        std::cout << "Evaluated " << numEvaluates << " nodes\n";
        std::cout << "Cache hits " << numCacheHits << ", misses " << numCacheMisses << "\n";
        board.ApplyMove(move);
        board.SwitchTurn();
        std::cout << "Computer played " << move << "\n";
        std::cout << board;
    }
}

void PlayComputerVsComputer() {
    Board board;
    SetDefaultBoard(board);

    std::cout << board;
    while (true) {
        numEvaluates = 0;
        auto move = FindBestMove(board, 6);
        //std::cout << "Evaluated " << numEvaluates << " nodes\n";
        board.ApplyMove(move);
        board.SwitchTurn();
        std::cout << board;
    }
}

void PlayHumanVsHuman() {
    Board board;
    SetDefaultBoard(board);

    std::cout << board;
    while (true) {
        auto move = INVALID_MOVE;
        while (move == INVALID_MOVE) {
            std::cout << "Your move: ";
            std::string moveString;
            std::cin >> moveString;
            move = ParseMove(moveString);
            if (!IsMoveValid(board, move)) {
                std::cout << "Invalid move\n";
                move = INVALID_MOVE;
            }
        }
        std::cout << "Your move " << move << "\n";
        board.ApplyMove(move);
        board.SwitchTurn();
        std::cout << board;
        std::cout << "Hash " << board.GetHash() << "\n";
    }
}


void Simulate() {
    Board board;
    SetDefaultBoard(board);

    std::cout << board;
    std::cout << "Hash " << board.GetHash() << "\n\n\n";

    const char* moves[4] = {"G1F3", "G8F6", "F3G1", "F6G8"};

    for (int i = 0; i < 4; i++) {
        auto move = ParseMove(moves[i]);
        std::cout << "m " << moves[i] << "\n";
        if (!IsMoveValid(board, move)) {
            std::cout << "Invalid move\n";
        }
            board.ApplyMove(move);
            board.SwitchTurn();
            std::cout << board;
            std::cout << "Hash " << board.GetHash() << "\n";
    }
}





int main()
{

    PlayLoop();
    //PlayHumanVsHuman();
    //PlayComputerVsComputer();
    //std::cout << "Hello World!\n";
}
