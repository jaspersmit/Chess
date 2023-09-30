#include <vector>

#include "TranspositionTable.h"


std::vector<TtEntry> transpositionTable;

auto InitializeTranspositionTable() -> bool {
	int numEntries = 512 * 1024 * 1024 / sizeof(TtEntry);
	transpositionTable.resize(numEntries);
	return true;
}

bool transpositionTableInitialed = InitializeTranspositionTable();


auto GetEntry(uint64_t hash) -> TtEntry* {
	return &transpositionTable[hash % transpositionTable.size()];
}