#include <SFML/Graphics.hpp>
#include "game.hpp"

#ifndef Board_H
#define Board_H

class Board {
    public:
        static const int width;
        static const int height;
        static const int tileSize;
        static void render(sf::RenderWindow& window, Game& game);
};

#endif