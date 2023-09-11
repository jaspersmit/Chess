#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include <cassert>

#include "Board.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "MoveOrder.h"
#include "Piece.h"


Move INVALID_MOVE = { {0,0}, {0,0} };

void SetDefaultBoard(Board& board) {
    for (int8_t r = 0; r < 8; r++)
        for (int8_t f = 0; f < 8; f++)
            board({ r, f }) = Piece::NO_PIECE;
    
    for (int8_t f = 0; f < 8; f++) {
        board({ 1, f }) = Piece::MY_PAWN;
        board({ 6, f }) = Piece::ENEMY_PAWN;
    }

    board({ 0, 0 }) = Piece::MY_ROOK;
    board({ 0, 1 }) = Piece::MY_KNIGHT;
    board({ 0, 2 }) = Piece::MY_BISHOP;
    board({ 0, 3 }) = Piece::MY_QUEEN;
    board({ 0, 4 }) = Piece::MY_KING;
    board({ 0, 5 }) = Piece::MY_BISHOP;
    board({ 0, 6 }) = Piece::MY_KNIGHT;
    board({ 0, 7 }) = Piece::MY_ROOK;
    board({ 7, 0 }) = Piece::ENEMY_ROOK;
    board({ 7, 1 }) = Piece::ENEMY_KNIGHT;
    board({ 7, 2 }) = Piece::ENEMY_BISHOP;
    board({ 7, 3 }) = Piece::ENEMY_QUEEN;
    board({ 7, 4 }) = Piece::ENEMY_KING;
    board({ 7, 5 }) = Piece::ENEMY_BISHOP;
    board({ 7, 6 }) = Piece::ENEMY_KNIGHT;
    board({ 7, 7 }) = Piece::ENEMY_ROOK;
}

std::ostream& operator<<(std::ostream& o, Piece piece) {
    switch (piece) {
    case Piece::NO_PIECE:
        o << ".";
        break;
    case Piece::MY_PAWN:
        o << "p";
        break;
    case Piece::MY_ROOK:
        o << "r";
        break;
    case Piece::MY_KNIGHT:
        o << "n";
        break;
    case Piece::MY_BISHOP:
        o << "b";
        break;
    case Piece::MY_QUEEN:
        o << "q";
        break;
    case Piece::MY_KING:
        o << "k";
        break;
    case Piece::ENEMY_PAWN:
        o << "P";
        break;
    case Piece::ENEMY_ROOK:
        o << "R";
        break;
    case Piece::ENEMY_KNIGHT:
        o << "N";
        break;
    case Piece::ENEMY_BISHOP:
        o << "B";
        break;
    case Piece::ENEMY_QUEEN:
        o << "Q";
        break;
    case Piece::ENEMY_KING:
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

int numEvaluates = 0;

int EvaluateBoard(const Board& board) {
    numEvaluates++;
    int score = 0;
    for (int8_t r = 7; r >= 0; r--) {
        for (int8_t f = 0; f < 8; f++) {
            switch (board({r, f})) {
            case Piece::NO_PIECE:
                break;
            case Piece::MY_PAWN:
                score += 1;
                break;
            case Piece::MY_ROOK:
                score += 5;
                break;
            case Piece::MY_KNIGHT:
                score += 3;
                break;
            case Piece::MY_BISHOP:
                score += 3;
                break;
            case Piece::MY_QUEEN:
                score += 9;
                break;
            case Piece::MY_KING:
                score += 1000000;
                break;
            case Piece::ENEMY_PAWN:
                score -= 1;
                break;
            case Piece::ENEMY_ROOK:
                score -= 5;
                break;
            case Piece::ENEMY_KNIGHT:
                score -= 3;
                break;
            case Piece::ENEMY_BISHOP:
                score -= 3;
                break;
            case Piece::ENEMY_QUEEN:
                score -= 9;
                break;
            case Piece::ENEMY_KING:
                score -= 1000000;
                break;
            }
        }
    }

    // check center squares
    for(int8_t rank = 3; rank <= 4; rank++)
        for (int8_t file = 3; file <= 4; file++) {
            auto square = Square{ rank, file };
            if (board.HasEnemy(square)) score--;
            if (board.HasFriend(square)) score++;
        }

    return score;
}

void ApplyMove(Board& board, const Move& move) {
    // Apply castling move
    if (board(move.from) == Piece::MY_KING && move.from == Square{ 0, 4 }) {
        if (move.to == Square{ 0, 2 }) {
            //Move rook
            board({ 0, 0 }) = Piece::NO_PIECE;
            board({ 0, 3 }) = Piece::MY_ROOK;
        }
        if (move.to == Square{ 0, 6 }) {
            //Move rook
            board({ 0, 7 }) = Piece::NO_PIECE;
            board({ 0, 5 }) = Piece::MY_ROOK;
        }
    }

    board(move.to) = board(move.from);
    board(move.from) = Piece::NO_PIECE;
    if (move.to.rank == 7 && board(move.to) == Piece::MY_PAWN)
        board(move.to) = Piece::MY_QUEEN;
}


int MinMax(Board& board, int depth, int alpha, int beta) {
    if (depth == 0) {
        return EvaluateBoard(board);
    }
    std::vector<Move> moves;
    GenerateMoves(board, moves);

    std::vector<int> indices(moves.size());
    std::iota(indices.begin(), indices.end(), 0);

    OrderMoves(board, moves, indices);

    auto maxScore = -MAX_SCORE;

    for (const auto& index : indices) {
        auto& move = moves[index];
        // Search no further if king was captured
        if (board(move.to) == Piece::ENEMY_KING) 
            return MAX_SCORE;

        Board nextBoard = board;
        ApplyMove(nextBoard, move);
        nextBoard.Invert();
        auto score = -MinMax(nextBoard, depth - 1, -beta, -alpha);
        if (score > alpha) alpha = score;
        if (score > beta) return beta;
        if (score > maxScore) maxScore = score;
    }

    return maxScore;
}

Move FindBestMove(Board& board, int depth) {
    std::vector<Move> moves;
    GenerateMoves(board, moves);

    auto maxMove = Move{ {0, 0},  {0, 0} };
    auto maxScore = -1000000;

    for (const auto& move : moves) {
        Board nextBoard = board;
        ApplyMove(nextBoard, move);
        //std::cout << nextBoard << "\n";
        nextBoard.Invert();
        //std::cout << "Inverted:\n";
        //std::cout << nextBoard << "\n\n\n";
        auto score = -MinMax(nextBoard, depth - 1, -1000000, 1000000);
        //std::cout << score << "\n";
        if (score > maxScore) {
            maxScore = score;
            maxMove = move;
        }
        std::cout << move << " score: " << score << "\n";
    }

    std::cout << "Score " << maxScore << "\n";
    return maxMove;
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
            piece = Piece::MY_PAWN;
            break;
        case 'r':
            piece = Piece::MY_ROOK;
            break;
        case 'n':
            piece = Piece::MY_KNIGHT;
            break;
        case 'b':
            piece = Piece::MY_BISHOP;
            break;
        case 'k':
            piece = Piece::MY_KING;
            break;
        case 'q':
            piece = Piece::MY_QUEEN;
            break;
        case 'P':
            piece = Piece::ENEMY_PAWN;
            break;
        case 'R':
            piece = Piece::ENEMY_ROOK;
            break;
        case 'N':
            piece = Piece::ENEMY_KNIGHT;
            break;
        case 'B':
            piece = Piece::ENEMY_BISHOP;
            break;
        case 'K':
            piece = Piece::ENEMY_KING;
            break;
        case 'Q':
            piece = Piece::ENEMY_QUEEN;
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
        ApplyMove(board, move);

        std::cout << board;

        board.Invert();
        numEvaluates = 0;
        move = FindBestMove(board, 5);
        std::cout << "Evaluated " << numEvaluates << " nodes\n";
        ApplyMove(board, move);
        board.Invert();
        std::cout << board;
    }

}

int main()
{
    PlayLoop();
    //std::cout << "Hello World!\n";

    Board board;
    ParseBoard(board,
        "Q...K..."
        "PPPPPPPP"
        "........"
        "........"
        "...N...."
        "........"
        "pppppppp"
        "q...k..."
    );



    std::cout << board << "\n";
    std::cout << "Best move " << FindBestMove(board, 5) << "\n";
}
