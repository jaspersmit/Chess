#include <cassert>
#include <iostream>

#include "MoveGenerator.h"
#include "Search.h"


#define ASSERT(x) AssertImpl(x, #x)

auto AssertImpl(bool condition, std::string check) {
	if (condition) return;
	std::cout << "Assertion " << check << " failed\n";
}


void TestCastling() {
	std::cout << "TestCastling\n";
	
	Board board;
	ParseBoard(board,
		"...K...."
		"........"
		"........"
		"........"
		"........"
		"........"
		"........"
		"....k..r"
	);
	ASSERT(IsMoveValid(board, ParseMove("E1G1")));

	// Cannot castle when in check
	ParseBoard(board,
		"...KQ..."
		"........"
		"........"
		"........"
		"........"
		"........"
		"........"
		"....k..r"
	);
	ASSERT(!IsMoveValid(board, ParseMove("E1G1")));
	
	ParseBoard(board,
		"...K.Q.."
		"........"
		"........"
		"........"
		"........"
		"........"
		"........"
		"....k..r"
	);
	ASSERT(!IsMoveValid(board, ParseMove("E1G1")));
	
	ParseBoard(board,
		"...K..Q."
		"........"
		"........"
		"........"
		"........"
		"........"
		"........"
		"....k..r"
	);
	ASSERT(!IsMoveValid(board, ParseMove("E1G1")));

	ParseBoard(board,
		"...K...."
		"........"
		"........"
		"........"
		"........"
		"........"
		"........"
		"r...k..r"
	);
	ASSERT(IsMoveValid(board, ParseMove("E1C1")));

	ParseBoard(board,
		"...KQ..."
		"........"
		"........"
		"........"
		"........"
		"........"
		"........"
		"r...k..r"
	);
	ASSERT(!IsMoveValid(board, ParseMove("E1C1")));

	ParseBoard(board,
		"...Q...K"
		"........"
		"........"
		"........"
		"........"
		"........"
		"........"
		"r...k..r"
	);
	ASSERT(!IsMoveValid(board, ParseMove("E1C1")));

	ParseBoard(board,
		"..Q....K"
		"........"
		"........"
		"........"
		"........"
		"........"
		"........"
		"r...k..r"
	);
	ASSERT(!IsMoveValid(board, ParseMove("E1C1")));

	ParseBoard(board,
		".Q.....K"
		"........"
		"........"
		"........"
		"........"
		"........"
		"........"
		"r...k..r"
	);
	ASSERT(IsMoveValid(board, ParseMove("E1C1")));

	ParseBoard(board,
		"Q......K"
		".P......"
		"........"
		"........"
		"........"
		"........"
		"........"
		"r...k..r"
	);
	ASSERT(IsMoveValid(board, ParseMove("E1C1")));

	// Test castling rights
	ParseBoard(theBoard,
		".......K"
		"........"
		"........"
		"........"
		"........"
		"........"
		"........"
		"r...k..r"
	);

	ASSERT(IsMoveValid(theBoard, ParseMove("E1G1")));
	ASSERT(theBoard.HasCastlingRights(CastlingSide::KING));
	ASSERT(IsMoveValid(theBoard, ParseMove("H1H2")));
	DoMove(ParseMove("H1H2"));
	ASSERT(!theBoard.HasCastlingRights(CastlingSide::KING));
	theBoard.SwitchTurn();
	DoMove(ParseMove("H8H7"));
	theBoard.SwitchTurn();
	DoMove(ParseMove("H2H1"));
	theBoard.SwitchTurn();
	ASSERT(!IsMoveValid(theBoard, ParseMove("E1G1")));
}


void TestMate() {
	ParseBoard(theBoard,
		"........"
		"........"
		"........"
		"........"
		"........"
		"....K..."
		"....Q..."
		"r...k..r"
	);
	ASSERT(IsInMate());
}

void TestEnPassant() {
	ParseBoard(theBoard,
		"....K..."
		"PPPPPPPP"
		"........"
		"....p..."
		"........"
		"........"
		"........"
		"....k..."
	);

	theBoard.SwitchTurn();
	DoMove(ParseMove("F7F5"));
	theBoard.SwitchTurn();
	ASSERT(IsMoveValid(theBoard, ParseMove("E5F6")));
	auto hash = theBoard.GetHash();
	DoMove(ParseMove("E5F6"));
	UndoMove();
	ASSERT(hash == theBoard.GetHash());




}

void Test() {
	TestCastling();
	TestMate();
	TestEnPassant();
}