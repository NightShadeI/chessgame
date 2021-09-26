#include <SFML/Graphics.hpp>
#include <memory>

#ifndef PIECE_H
#define PIECE_H

using namespace std;

enum PieceName {
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING
};

class Game;
struct Move;

class Piece {
    public:
        Piece(int x, int y, int ty);
        int xPos;
        int yPos;
        int dragOffsetX;
        int dragOffsetY;
        int type; // -1 for black and 1 for white
        sf::Texture myTexture;
        void loadTexture();
        void render(sf::RenderWindow& window);
        void doMove(Game& game, Move& move, bool undo = false);
        virtual vector<unique_ptr<Move>> getMoves(Game& game);
        void setDrag(int deltaX, int deltaY);
        bool canDoMove(Game& game, int newX, int newY);
        bool vigorousCanDoMove(Game& game, int newX, int newY);
        virtual bool isValidMove(Game& game, int newX, int newY);
        virtual std::string getPieceName();
        virtual PieceName getPieceType();
        virtual bool isSlidingPiece();
        virtual int getPieceValue();
        virtual int getXTranslation();
        virtual void setup(Game& game);
        virtual void cleanThreats(Game& game);
        virtual void updateThreats(Game& game, int newX, int newY, Piece* captured);
        std::string getPiecePath();
};

#endif