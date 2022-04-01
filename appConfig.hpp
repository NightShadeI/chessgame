#include <string>
#include <vector>

#include "game.hpp"

#ifndef CONFIG_H
#define CONFIG_H

extern std::string DEFAULT_FEN;
void setupBoard(Game& game, std::string& FENstring);
void setupHashes(Game& game);

#endif