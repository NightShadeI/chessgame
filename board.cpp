#include "board.hpp"
#include "game.hpp"
#include "pieces/piece.hpp"
#include "move.hpp"

using namespace std;
using namespace sf;

void Board::render(RenderWindow& window, Game& game) {
    RectangleShape chessTile;
    RectangleShape lastMoveTile;
    lastMoveTile.setFillColor(Color(198, 147, 202, 150));
    lastMoveTile.setSize(Vector2f(Board::tileSize, Board::tileSize));
    chessTile.setSize(Vector2f(Board::tileSize, Board::tileSize));
    for (int r = 0; r < Board::height; r++) {
        for (int c = 0; c < Board::width; c++) {
            Color tileColour = (r + c) % 2 ? Color(171, 99, 42) : Color(242, 214, 178);
            chessTile.setFillColor(tileColour);
            chessTile.setPosition(Board::tileSize * c, Board::tileSize * r);
            window.draw(chessTile);
            Piece* pieceAt = game.getPieceAt(c, r);
        }
    }
    if (game.moveHistory.size()) {
        unique_ptr<Move>& lastMove = game.moveHistory[game.moveHistory.size() - 1];
        lastMoveTile.setPosition(Board::tileSize * lastMove->xFrom, Board::tileSize * lastMove->yFrom);
        window.draw(lastMoveTile);
        lastMoveTile.setPosition(Board::tileSize * lastMove->xTo, Board::tileSize * lastMove->yTo);
        window.draw(lastMoveTile);
    }
}