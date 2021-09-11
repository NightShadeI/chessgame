#include <string>
#include <vector>
#include <unordered_set>
#include "game.hpp"

#ifndef CONFIG_H
#define CONFIG_H

extern std::string DEFAULT_FEN;
void setupBoard(Game& game, std::string& FENstring);
vector<vector<unordered_set<Piece*>>> setupThreatMap(Game& game);

#endif