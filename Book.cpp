#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>

#include "Board.h"
#include "MoveGenerator.h"
#include "Util.h"


struct MoveAndCount {
	Move move;
	int count;
};

std::unordered_map<uint64_t, std::vector<MoveAndCount>> book;

void RewriteBook();

void ReadBook() {
	std::ifstream is("NewBook.txt");
	std::string line;

	int count = 0;
	uint64_t hash = 0;

	auto startTime = std::chrono::high_resolution_clock::now();

	while (is.good()) {
		std::getline(is, line);

		if (line.starts_with("pos")) {
			Board board;

			ParseFENBoard(board, line);
			hash = board.GetHash();

			if (book.contains(hash)) {
				std::cerr << "Book contains duplicates\n";
				std::cerr << line << "\n";
				std::exit(1);
			}
		}
		else if (line == "") break;
		else {
			auto parts = Split(line, ' ');
			if (parts.size() != 2) {
				std::cerr << "Invalid move line\n";
				std::cerr << line;
				std::exit(1);
			}
			auto move = ParseMove(parts[0]);
			auto count = std::stoi(parts[1]);
			book[hash].push_back({ move, count });
		}
	}
	auto endTime = std::chrono::high_resolution_clock::now();
	//RewriteBook();
	//std::cout << "Book parsed in " << std::chrono::duration<double, std::milli>(endTime - startTime).count() << "\n";
}

std::random_device randomDevice;
std::mt19937 randomGenerator(randomDevice());


std::optional<Move> GetBookMove(const Board& board) {
	auto it = book.find(board.GetHash());
	if (it == book.end()) return {};
	int total = 0;
	for (auto& move : it->second) {
		total += move.count;
	}

	std::uniform_int_distribution<> distribution(0, total);
	int random = distribution(randomGenerator);

	for (auto& move : it->second) {
		if (random < move.count) {
			return move.move;
		}
		else {
			random -= move.count;
		}
	}
	return {};
}

void RewriteRecursive(std::ostream& os, std::unordered_map<uint64_t, std::vector<MoveAndCount>>& newBook) {
	auto boardWithoutEnPassent = theBoard;
	boardWithoutEnPassent.SetEnPassentFile(INVALID_ENPASSENT_FILE);
	auto it = book.find(boardWithoutEnPassent.GetHash());
	if (it != book.end()) {

		if (newBook.contains(theBoard.GetHash())) return;

		newBook.insert({ theBoard.GetHash(), it->second });
		if (it != book.end()) {
			newBook.insert({ theBoard.GetHash(), it->second });
			os << "pos " << FormatFENBoard(theBoard) << "\n";
			for (auto& move : it->second)
				os << MoveToUCI(move.move) << " " << move.count << "\n";
		}

		MoveList moveList;
		GenerateMoves(theBoard, moveList);
		for (auto& move : moveList) {
			DoMove(move);
			theBoard.SwitchTurn();
			RewriteRecursive(os, newBook);
			theBoard.SwitchTurn();
			UndoMove();
		}
	}
}

void RewriteBook() {
	SetDefaultBoard(theBoard);
	MoveList moveList;
	std::unordered_map<uint64_t, std::vector<MoveAndCount>> newBook;
	std::fstream os("NewBook.txt", std::fstream::out);
	RewriteRecursive(os, newBook);
}