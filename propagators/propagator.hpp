#include "../pieces/piece.hpp"
#include "../game.hpp"
#include "../move.hpp"

#ifndef PROPAGATOR_H
#define PROPGAGATOR_H

class Propagator {
    public:
        Propagator(Piece* rootPiece, Move* movePerformed);
        Piece* rootPiece;
        Move* movePerformed;
        virtual void propagate(Game& game);
        void updater(Game& game, int xStart, int yStart, int xDir, int yDir, bool threaten);
        inline static Propagator fetchPropagator(Piece* queryPiece, Move* queryMove, bool updateFrom);
};

#endif