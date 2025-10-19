#include "utilite.h"

bool operator==(std::pair<int,int> pair, std::pair<int,int> pair2) { return pair.first == pair2.first && pair.second == pair2.second; };
bool operator==(std::pair<int,int> pair, int value) { return pair.first == value && pair.second == value; };
bool operator!=(std::pair<int,int> pair, std::pair<int,int> pair2) { return pair.first != pair2.first && pair.second != pair2.second; };
bool operator!=(std::pair<int,int> pair, int value) { return pair.first != value && pair.second != value; };
