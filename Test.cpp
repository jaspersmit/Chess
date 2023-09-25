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

}

void Test() {
	TestCastling();
}