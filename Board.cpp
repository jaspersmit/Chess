#include <sstream>
#include <vector>

#include "Board.h"
#include "Util.h"
#include "Zobrist.h"

enum class SpecialMove {
    NORMAL_MOVE,
    INVALIDATE_CASTLING,
    INVALIDATE_CASTLING_KING_SIDE,
    INVALIDATE_CASTLING_QUEEN_SIDE,
    EN_PASSANT
};

struct HistoricMove {
    Square from;
    Square to;
    Piece capturedPiece;
    bool promotion;
    SpecialMove specialMove;
    int8_t previousEnPassentFile;
};

std::vector<HistoricMove> history;

Board theBoard;

void DoMove(const Move& move) {
    auto specialMove = SpecialMove::NORMAL_MOVE;

    // Apply castling move
    Piece king;
    Piece rook;
    Piece pawn;
    Piece queen;
    int8_t kingRank;
    int8_t pawnRank;
    int8_t pawn2MoveRank;
    int8_t previousEnPassentFile = theBoard.GetEnPassentFile();

    if (theBoard.GetTurn() == Color::WHITE) {
        king = Piece::WHITE_KING;
        rook = Piece::WHITE_ROOK;
        pawn = Piece::WHITE_PAWN;
        queen = Piece::WHITE_QUEEN;
        kingRank = 0;
        pawnRank = 1;
        pawn2MoveRank = 3;
    }
    else {
        king = Piece::BLACK_KING;
        rook = Piece::BLACK_ROOK;
        pawn = Piece::BLACK_PAWN;
        queen = Piece::BLACK_QUEEN;
        kingRank = 7;
        pawnRank = 6;
        pawn2MoveRank = 4;
    }

    if (theBoard(move.from) == king) {
        if (theBoard.HasCastlingRights(CastlingSide::KING))
            if (theBoard.HasCastlingRights(CastlingSide::QUEEN))
                specialMove = SpecialMove::INVALIDATE_CASTLING;
            else 
                specialMove = SpecialMove::INVALIDATE_CASTLING_KING_SIDE;
        else if (theBoard.HasCastlingRights(CastlingSide::QUEEN))
            specialMove = SpecialMove::INVALIDATE_CASTLING_QUEEN_SIDE;

        if (move.from == Square{ kingRank, 4 }) {
            if (move.to == Square{ kingRank, 2 }) {
                //Move rook
                theBoard.SetSquare({ kingRank, 0 }, Piece::NO_PIECE);
                theBoard.SetSquare({ kingRank, 3 }, rook);
            }
            if (move.to == Square{ kingRank, 6 }) {
                //Move rook
                theBoard.SetSquare({ kingRank, 7 }, Piece::NO_PIECE);
                theBoard.SetSquare({ kingRank, 5 }, rook);
            }
        }
    }

    if (theBoard(move.from) == rook) {
        if (move.from == Square{ kingRank, 0 } && theBoard.HasCastlingRights(CastlingSide::QUEEN)) {
            specialMove = SpecialMove::INVALIDATE_CASTLING_QUEEN_SIDE;
            theBoard.SetCastlingRights(CastlingSide::QUEEN, false);
        } 
        else if (move.from == Square{ kingRank, 7 } && theBoard.HasCastlingRights(CastlingSide::KING)) {
            specialMove = SpecialMove::INVALIDATE_CASTLING_KING_SIDE;
            theBoard.SetCastlingRights(CastlingSide::KING, false);
        }
    }

    // Check enpassent
    if (theBoard(move.from) == pawn
        && move.from.file != move.to.file
        && theBoard.IsEmpty(move.to)) {
        specialMove = SpecialMove::EN_PASSANT;
        // Clear out captured piece 
        theBoard.SetSquare({ move.from.rank, move.to.file }, Piece::NO_PIECE);
    }

    // Double move allows en passant on the next move
    if (theBoard(move.from) == pawn && move.from.rank == pawnRank && move.to.rank == pawn2MoveRank) {
        theBoard.SetEnPassentFile(move.to.file);
    }
    else {
        theBoard.SetEnPassentFile(INVALID_ENPASSENT_FILE);
    }

    auto piece = theBoard(move.from);
    auto capturedPiece = theBoard(move.to);

    theBoard.SetSquare(move.from, Piece::NO_PIECE);
    theBoard.SetSquare(move.to, piece);

    auto promotion = false;
    if (move.to.rank == (7 - kingRank) && theBoard(move.to) == pawn) {
        theBoard.SetSquare(move.to, queen);
        promotion = true;
    }

    history.push_back({ move.from, move.to, capturedPiece, promotion, specialMove, previousEnPassentFile });
}

void UndoMove() {
    auto move = history.back();
    history.pop_back();

    // Apply castling move
    Piece king;
    Piece rook;
    Piece pawn;
    int8_t kingRank;

    if (theBoard.GetTurn() == Color::WHITE) {
        king = Piece::WHITE_KING;
        rook = Piece::WHITE_ROOK;
        pawn = Piece::WHITE_PAWN;
        kingRank = 0;
    }
    else {
        king = Piece::BLACK_KING;
        rook = Piece::BLACK_ROOK;
        pawn = Piece::BLACK_PAWN;
        kingRank = 7;
    }

    if (theBoard(move.to) == king && move.from == Square{ kingRank, 4 }) {
        if (move.to == Square{ kingRank, 2 }) {
            //Move rook
            theBoard.SetSquare({ kingRank, 0 }, rook);
            theBoard.SetSquare({ kingRank, 3 }, Piece::NO_PIECE);
        }
        if (move.to == Square{ kingRank, 6 }) {
            //Move rook
            theBoard.SetSquare({ kingRank, 7 }, rook);
            theBoard.SetSquare({ kingRank, 5 }, Piece::NO_PIECE);
        }
    }

    auto piece = theBoard(move.to);

    theBoard.SetSquare(move.from, piece);
    theBoard.SetSquare(move.to, move.capturedPiece);

    if (move.promotion)
        theBoard.SetSquare(move.from, pawn);

    switch (move.specialMove) {
    case SpecialMove::INVALIDATE_CASTLING:
        theBoard.SetCastlingRights(CastlingSide::QUEEN, true);
        theBoard.SetCastlingRights(CastlingSide::KING, true);
        break;
    case SpecialMove::INVALIDATE_CASTLING_KING_SIDE:
        theBoard.SetCastlingRights(CastlingSide::KING, true);
        break;
    case SpecialMove::INVALIDATE_CASTLING_QUEEN_SIDE:
        theBoard.SetCastlingRights(CastlingSide::QUEEN, true);
        break;
    case SpecialMove::EN_PASSANT:
        // Place back pawn
        auto capturedPawn = theBoard.GetTurn() == Color::WHITE ? Piece::BLACK_PAWN : Piece::WHITE_PAWN;
        theBoard.SetSquare({ move.from.rank, move.to.file }, capturedPawn);
        assert(move.previousEnPassentFile != INVALID_ENPASSENT_FILE);    
        break;
    }

    theBoard.SetEnPassentFile(move.previousEnPassentFile);
}

void ParseBoard(Board& board, const std::string& str) {
    if (str.length() != 64) {
        std::cerr << "Could not parse board";
        std::exit(1);
    }

    board.Reset();

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
        board.SetSquare({ rank, file }, piece);
    }
}

void ParseFENBoard(Board& board, const std::string& fen) {
    board.Reset();
    auto parts = Split(fen, ' ');

    if (parts.size() < 5) {
        std::cerr << "Invalid FEN board\n";
        std::exit(1);
    }

    auto& boardStr = parts[1];
    auto ranks = Split(boardStr, '/');

    if (ranks.size() != 8) {
        std::cerr << "Invalid FEN board, boards must have 8 ranks \n";
        std::exit(1);
    }

    for (int8_t rank = 0; rank < 8; rank++) {
        auto rankString = ranks[rank];
        int8_t file = 0;
        size_t pos = 0;
        while (file < 8 && pos < rankString.length()) {
            auto c = rankString[pos++];
            if (c >= '1' && c <= '9') {
                file += (c - '0');
            }
            else {
                auto piece = Piece::NO_PIECE;
                switch (c) {
                case 'r':
                    piece = Piece::BLACK_ROOK;
                    break;
                case 'n':
                    piece = Piece::BLACK_KNIGHT;
                    break;
                case 'b':
                    piece = Piece::BLACK_BISHOP;
                    break;
                case 'k':
                    piece = Piece::BLACK_KING;
                    break;
                case 'q':
                    piece = Piece::BLACK_QUEEN;
                    break;
                case 'p':
                    piece = Piece::BLACK_PAWN;
                    break;
                case 'R':
                    piece = Piece::WHITE_ROOK;
                    break;
                case 'N':
                    piece = Piece::WHITE_KNIGHT;
                    break;
                case 'B':
                    piece = Piece::WHITE_BISHOP;
                    break;
                case 'K':
                    piece = Piece::WHITE_KING;
                    break;
                case 'Q':
                    piece = Piece::WHITE_QUEEN;
                    break;
                case 'P':
                    piece = Piece::WHITE_PAWN;
                    break;
                }
                board.SetSquare(Square{ 7 - rank, file }, piece);
                file++;
            }
        }
    }

    auto color = parts[2];
    if (color == "w") {
        board.SetTurn(Color::WHITE);
    }
    else if (color == "b") {
        board.SetTurn(Color::BLACK);
    }
    else {
        std::cerr << "Invalid FEN color\n";
        std::exit(1);
    }

    auto castlingRights = parts[3];
    board.SetCastlingRights(Color::WHITE, CastlingSide::KING, false);
    board.SetCastlingRights(Color::WHITE, CastlingSide::QUEEN, false);
    board.SetCastlingRights(Color::BLACK, CastlingSide::KING, false);
    board.SetCastlingRights(Color::BLACK, CastlingSide::QUEEN, false);
    for (auto c : castlingRights) {
        switch (c) {
        case 'K':
            board.SetCastlingRights(Color::WHITE, CastlingSide::KING, true);
            break;
        case 'Q':
            board.SetCastlingRights(Color::WHITE, CastlingSide::QUEEN, true);
            break;
        case 'k':
            board.SetCastlingRights(Color::BLACK, CastlingSide::KING, true);
            break;
        case 'q':
            board.SetCastlingRights(Color::BLACK, CastlingSide::QUEEN, true);
            break;
        }
    }

    auto enPassent = parts[4];
    if (enPassent != "-") {
        if (enPassent.length() != 2) {
            std::cerr << "Invalid enpassent for fen string\n";
            std::exit(1);
        }
        board.SetEnPassentFile(enPassent[0] - 'a');
    }
}

std::string FormatFENBoard(Board& board) {
    std::stringstream ss;
    for (int8_t rank = 7; rank >= 0; rank--) {
        int8_t file = 0;
        int empty = 0;
        for (int8_t file = 0; file < 8; file++) {
            auto piece = board({ rank, file });
            char c = '.';
            switch (piece) {
            case Piece::NO_PIECE:
                c = '.';
                break;
            case Piece::WHITE_PAWN:
                c = 'P';
                break;
            case Piece::WHITE_ROOK:
                c = 'R';
                break;
            case Piece::WHITE_KNIGHT:
                c = 'N';
                break;
            case Piece::WHITE_BISHOP:
                c = 'B';
                break;
            case Piece::WHITE_QUEEN:
                c = 'Q';
                break;
            case Piece::WHITE_KING:
                c = 'K';
                break;
            case Piece::BLACK_PAWN:
                c = 'p';
                break;
            case Piece::BLACK_ROOK:
                c = 'r';
                break;
            case Piece::BLACK_KNIGHT:
                c = 'n';
                break;
            case Piece::BLACK_BISHOP:
                c = 'b';
                break;
            case Piece::BLACK_QUEEN:
                c = 'q';
                break;
            case Piece::BLACK_KING:
                c = 'k';
                break;
            }
            if (c == '.') {
                empty++;
            }
            else {
                if (empty > 0) {
                    ss << empty;
                }
                empty = 0;
                ss << c;
            }
        }
        if (empty > 0)
            ss << empty;
        
        if (rank > 0)
            ss << "/";
    }

    ss << " " << (board.GetTurn() == Color::WHITE ? "w" : "b") << " ";
    bool rights = false;
    if (board.HasCastlingRights(Color::WHITE, CastlingSide::KING)) {
        rights = true;
        ss << "K";
    }
    if (board.HasCastlingRights(Color::WHITE, CastlingSide::QUEEN)) {
        rights = true;
        ss << "Q";
    }
    if (board.HasCastlingRights(Color::BLACK, CastlingSide::KING)) {
        rights = true;
        ss << "k";
    }
    if (board.HasCastlingRights(Color::BLACK, CastlingSide::QUEEN)) {
        rights = true;
        ss << "q";
    }
    if (!rights) ss << "-";
    
    ss << " ";

    if (board.GetEnPassentFile() == INVALID_ENPASSENT_FILE)
        ss << "-";
    else
        ss << static_cast<char>(board.GetEnPassentFile() + 'a') << (board.GetTurn() == Color::WHITE ? '3' : '6');

    return ss.str();
}

void SetDefaultBoard(Board& board) {
    ParseBoard(board,
        "RNBQKBNR"
        "PPPPPPPP"
        "........"
        "........"
        "........"
        "........"
        "pppppppp"
        "rnbqkbnr"
    );
}

std::string GetProtocolString(const Board& board) {
    std::stringstream ss;
    for (int8_t rank = 7; rank >= 0; rank--) {
        for (int8_t file = 0; file < 8; file++) {
            ss << board(Square{ rank, file });
        }
    }
    return ss.str();
}