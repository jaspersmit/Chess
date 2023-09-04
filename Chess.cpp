#include <algorithm>
#include <iostream>
#include <vector>
#include <cassert>

enum class Piece : uint8_t {
    NO_PIECE,
    MY_PAWN,
    MY_ROOK,
    MY_KNIGHT,
    MY_BISHOP,
    MY_QUEEN,
    MY_KING,
    ENEMY_PAWN,
    ENEMY_ROOK,
    ENEMY_KNIGHT,
    ENEMY_BISHOP,
    ENEMY_QUEEN,
    ENEMY_KING,
};

class Direction {
public:
    int8_t vertical;
    int8_t horizontal;
};

class Square {
public:
    int8_t rank;
    int8_t file;

     Square(int8_t rank, int8_t file) :
        rank(rank), file(file) {

    }

    auto operator+ (const Square& other) const -> Square {
        return { rank + other.rank, file + other.file };
    }

    auto operator == (const Square& other) const -> bool {
        return other.file == file && other.rank == rank;
    }

    auto Add(int8_t r, int8_t f) const-> Square {
        return { rank + r, file + f };
    }

    auto Add(Direction direction) const-> Square {
        return { rank + direction.vertical, file + direction.horizontal };
    }

    auto IsValid() const -> bool {
        return rank >= 0 && rank < 8 && file >= 0 && file < 8;
    }
};

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

class Move {
public:
    Square from;
    Square to;
    Piece piece;
    Piece capture;

    auto operator==(const Move& other) const -> bool {
        return other.from == from && other.to == to;
    }
};

Move INVALID_MOVE = { {0,0}, {0,0}, Piece::NO_PIECE, Piece::NO_PIECE };

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

int EvaluateBoard(const Board& board) {
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

#define GEN_MOVE(target) moves.push_back({ from, target, board(from), board(target) })

void GeneratePawnMoves(const Board& board, std::vector<Move>& moves, Square from) {
    auto up1 = from.Add(1, 0);
    if (board.IsEmpty(up1)) {
        if (from.rank == 6) {
            // Special treat promotion
        }
        moves.push_back({ from, up1 });
        if (from.rank == 1) {
            auto up2 = up1.Add(1, 0);
            if (board(up2) == Piece::NO_PIECE)
                GEN_MOVE(up2);
        }
    }

    // Capturing moves
    if (from.file > 0) {
        auto left = up1.Add(0, -1);
        if (board.HasEnemy(left)) {
            GEN_MOVE(left);
        }
    }
    if (from.file < 7) {
        auto right = up1.Add(0, 1);
        if (board.HasEnemy(right)) {
            GEN_MOVE(right);
        }
    }
}

Direction knightMoves[8] = {
    {-1, -2},
    {-2, -1},
    {1, -2},
    {2, -1},
    {1, 2},
    {2, 1},
    {-1, 2},
    {-2, 1}
};

void GenerateKnightMoves(const Board& board, std::vector<Move>& moves, Square from) {
    for (int i = 0; i < 8; i++) {
        auto to = from.Add(knightMoves[i]);
        if (to.IsValid() && !board.HasFriend(to)) {

            //std::cout << from << " > " << to << "\n";
            //auto& km = knightMoves[i];
            //std::cout << (int)km.vertical << ", " << (int)km.horizontal << "\n";

            GEN_MOVE(to);
        }
    }
}

Direction bishopDirections[] = {
    {-1, -1},
    {1, -1},
    {1, 1},
    {-1, 1}
};

void GenerateBishopMoves(const Board& board, std::vector<Move>& moves, Square from) {
    for (int i = 0; i < 4; i++) {
        auto direction = bishopDirections[i];
        auto to = from;
        while (true) {
            to = to.Add(direction);
            if (!to.IsValid()) break;
            if (board.HasFriend(to)) break;
            GEN_MOVE(to);
            if (board.HasEnemy(to)) break;
        }
    }
}

Direction rookDirections[] = {
    {0, 1},
    {1, 0},
    {-1, 0},
    {0, -1}
};

void GenerateRookMoves(const Board& board, std::vector<Move>& moves, Square from) {
    for (int i = 0; i < 4; i++) {
        auto direction = rookDirections[i];
        auto to = from;
        while (true) {
            to = to.Add(direction);
            if (!to.IsValid()) break;
            if (board.HasFriend(to)) break;
            GEN_MOVE(to);
            if (board.HasEnemy(to)) break;
        }
    }
}

void GenerateQueenMoves(const Board& board, std::vector<Move>& moves, Square from) {
    GenerateBishopMoves(board, moves, from);
    GenerateRookMoves(board, moves, from);
}

Direction kingDirections[] = {
    {0, 1},
    {1, 0},
    {-1, 0},
    {0, -1},
    {-1, -1},
    {1, -1},
    {1, 1},
    {-1, 1}
};

void GenerateKingMoves(const Board& board, std::vector<Move>& moves, Square from) {
    for (int i = 0; i < 8; i++) {
        Square to = from.Add(kingDirections[i]);
        if (to.IsValid() && !board.HasFriend(to)) {
            GEN_MOVE(to);
        }
    }
    //Castling
    if (from == Square {0, 4}) {
        if (board({ 0, 0 }) == Piece::MY_ROOK
            && board({ 0, 1 }) == Piece::NO_PIECE
            && board({ 0, 2 }) == Piece::NO_PIECE
            && board({ 0, 3 }) == Piece::NO_PIECE) {
            Square to = { 0, 2 };
            GEN_MOVE(to);
        }
        if (board({ 0, 7 }) == Piece::MY_ROOK
            && board({ 0, 5 }) == Piece::NO_PIECE
            && board({ 0, 6 }) == Piece::NO_PIECE) {
            Square to = { 0, 6 };
            GEN_MOVE(to);
        }
    }

}

void GenerateMoves(const Board& board, std::vector<Move>& moves) {
    for (int8_t r = 7; r >= 0; r--) {
        for (int8_t f = 0; f < 8; f++) {
            auto square = Square { r, f };
            switch (board(square)) {
            case Piece::MY_PAWN:
                GeneratePawnMoves(board, moves, square);
                break;
            case Piece::MY_ROOK:
                GenerateRookMoves(board, moves, square);
                break;
            case Piece::MY_KNIGHT:
                GenerateKnightMoves(board, moves, square);
                break;
            case Piece::MY_BISHOP:
                GenerateBishopMoves(board, moves, square);
                break;
            case Piece::MY_QUEEN:
                GenerateQueenMoves(board, moves, square);
                break;
            case Piece::MY_KING:
                GenerateKingMoves(board, moves, square);
                break;
            }
        }
    }
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

constexpr int MAX_SCORE = 1000000;

int MinMax(Board& board, int depth, int alpha, int beta) {
    if (depth == 0) {
        return EvaluateBoard(board);
    }
    std::vector<Move> moves;
    GenerateMoves(board, moves);

    std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        if (a.capture > b.capture)
            return 1;
        if (a.capture < b.capture)
            return -1;
        if (a.piece > b.piece)
            return 1;
        if (a.piece < b.piece)
            return -1;
        return 0;
        });


    auto maxScore = -MAX_SCORE;

    for (const auto& move : moves) {
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
        move = FindBestMove(board, 5);
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
