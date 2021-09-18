#include "board.hpp"
#include "threatTile.hpp"
#include "pieces/piece.hpp"
#include "move.hpp"

using namespace std;
using namespace sf;

const int Board::width = 8;
const int Board::height = 8;
const int Board::tileSize = 80;

void Board::render(RenderWindow& window, Game& game) {
    RectangleShape chessTile;
    RectangleShape lastMoveTile;
    CircleShape threatCircle;
    lastMoveTile.setFillColor(Color(198, 147, 202, 150));
    lastMoveTile.setSize(Vector2f(Board::tileSize, Board::tileSize));
    chessTile.setSize(Vector2f(Board::tileSize, Board::tileSize));
    threatCircle.setRadius(30);
    for (int r = 0; r < Board::height; r++) {
        for (int c = 0; c < Board::width; c++) {
            Color tileColour = (r + c) % 2 ? Color(171, 99, 42) : Color(242, 214, 178);
            chessTile.setFillColor(tileColour);
            chessTile.setPosition(Board::tileSize * c, Board::tileSize * r);
            window.draw(chessTile);
            ThreatTile* tile = game.threatMap[r][c];
            Piece* pieceAt = game.getPieceAt(c, r);
            if (game.useThreatMap && (!pieceAt || pieceAt->type == 1) && tile->blackCount > 0) {
                threatCircle.setFillColor(Color(255, 0, 0, 100));
                threatCircle.setPosition(Board::tileSize * c + 10, Board::tileSize * r + 10);
                window.draw(threatCircle);
            }
        }
    }
    if (game.moveHistory.size()) {
        Move* lastMove = game.moveHistory[game.moveHistory.size() - 1];
        lastMoveTile.setPosition(Board::tileSize * lastMove->xFrom, Board::tileSize * lastMove->yFrom);
        window.draw(lastMoveTile);
        lastMoveTile.setPosition(Board::tileSize * lastMove->xTo, Board::tileSize * lastMove->yTo);
        window.draw(lastMoveTile);
    }
}