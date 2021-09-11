#include "../pieces/piece.hpp"
#include "../game.hpp"

#ifndef PROPAGATOR_H
#define PROPGAGATOR_H

class Propagator {
    public:
        Propagator(Piece* rootPiece, Piece* targetPiece);
        virtual void propagate(Game& game);
};

#endif