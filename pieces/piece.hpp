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
        inline virtual std::string getPieceName() { return "defaultName"; }
        inline virtual PieceName getPieceType() { return PieceName::PAWN; }
        inline virtual bool isSlidingPiece() { return false; }
        inline virtual int getPieceValue() { return 0; }
        inline virtual int getXTranslation() { return 10; }
        inline std::string getPiecePath() {
            std::string pieceColour = type == -1 ? "black" : "white";
            return "img/" + pieceColour + getPieceName() + ".png";
        }
        virtual void setup(Game& game);
        virtual void cleanThreats(Game& game);
        virtual void updateThreats(Game& game, int newX, int newY, Piece* captured);
};

#endif