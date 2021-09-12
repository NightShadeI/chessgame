#include "../pieces/piece.hpp"
#include "../game.hpp"
#include "../move.hpp"

#ifndef PROPAGATOR_H
#define PROPAGATOR_H

class Propagator {
    public:
        Propagator(Piece* rootPiece, Move* movePerformed);
        Piece* rootPiece;
        Move* movePerformed;
        virtual void openPropagation(Game& game);
        virtual void closePropagation(Game& game);
        void updater(Game& game, int xStart, int yStart, int xDir, int yDir, bool threaten);
        static Propagator* fetchPropagator(Piece* queryPiece, Move* queryMove, bool updateFrom);
};

#endif