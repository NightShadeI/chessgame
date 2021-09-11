#include <unordered_set>
#include "pieces/piece.hpp"

#ifndef THREAT_TILE_H
#define THREAT_TILE_H

struct ThreatTile
{
    std::unordered_set<Piece*> threatening;
    int blackCount;
    int whiteCount;
};

#endif