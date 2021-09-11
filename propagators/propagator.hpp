#include "../pieces/piece.hpp"

#ifndef PROPAGATOR_H
#define PROPGAGATOR_H

class Propagator {
    public:
        Propagator(Piece* rootPiece, Piece* targetPiece);
        virtual void propagate();
};

#endif