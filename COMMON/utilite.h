#ifndef UTILITE_H
#define UTILITE_H
#include <QString>

bool operator==(std::pair<int,int> pair, std::pair<int,int> pair2);
bool operator!=(std::pair<int,int> pair, std::pair<int,int> pair2);
bool operator==(std::pair<int,int> pair, int value);
bool operator!=(std::pair<int,int> pair, int value);
#endif 
