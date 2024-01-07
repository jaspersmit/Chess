#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Board.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "Search.h"

int main() {

	while (true) {
		std::string line;
		std::getline(std::cin, line);

		std::vector<std::string> arguments;
		std::string arg;
		std::istringstream iss(line);

		while (iss >> arg) {
			arguments.push_back(arg);
		}

		if (arguments.empty()) break;

		std::string command = arguments[0];

		if (command == "uci") {
			std::cout << "info name JChess\n";
			std::cout << "info author Jasper Smit\n";
			std::cout << "uciok\n";
		} else if (command == "isready") {
			std::cout << "readyok\n";
		}
		else if (command == "ucinewgame") {
			// Do nothing
		}
		else if (command == "position") {
			if (arguments.size() >= 2 && arguments[1] == "startpos") {
				SetDefaultBoard(theBoard);
			}
			if (arguments.size() >= 3 && arguments[2] == "moves") {
				for (int i = 3; i < arguments.size(); i++) {
					auto move = ParseMove(arguments[i]);
					if (!IsMoveValid(theBoard, move)) {
						std::cout << "Invalid move\n";
						move = INVALID_MOVE;
					}
					else {
						DoMove(move);
						theBoard.SwitchTurn();
					}
				}
			}

		}
		else if (command == "go") {
			auto move = FindBestMoveInTime();
			DoMove(move);
			theBoard.SwitchTurn();
			std::cout << "bestmove " << MoveToUCI(move) << "\n";

		}
		else {
			std::cout << "unknown command " << command << "\n";
			break;
		}
	}

}