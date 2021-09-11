#include "board.hpp"
#include <iostream>

using namespace std;
using namespace sf;

const int Board::width = 8;
const int Board::height = 8;
const int Board::tileSize = 80;

void Board::render(RenderWindow& window, Game& game) {
    RectangleShape chessTile;
    CircleShape threatCircle;
    chessTile.setSize(Vector2f(Board::tileSize, Board::tileSize));
    threatCircle.setRadius(30);
    for (int r = 0; r < Board::height; r++) {
        for (int c = 0; c < Board::width; c++) {
            Color tileColour = (r + c) % 2 ? Color(171, 99, 42) : Color(242, 214, 178);
            chessTile.setFillColor(tileColour);
            chessTile.setPosition(Board::tileSize * c, Board::tileSize * r);
            window.draw(chessTile);
            // if (game.threatMap[r][c].size()) {
            //     threatCircle.setFillColor(Color(255, 0, 0, 100));
            //     threatCircle.setPosition(Board::tileSize * c + 10, Board::tileSize * r + 10);
            //     window.draw(threatCircle);
            // }
        }
    }
}