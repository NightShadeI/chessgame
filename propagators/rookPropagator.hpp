#include "propagator.hpp"

#ifndef ROOK_PROPAGATOR_H
#define ROOK_PROPAGATOR_H

class RookPropagator : public Propagator {
    public:
        RookPropagator(Piece* rootPiece, Piece* targetPiece);
        void propagate(Game& game) override;
};

#endif