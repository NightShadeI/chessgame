#include <SFML/Graphics.hpp>

#ifndef Board_H
#define Board_H

class Game;

class Board {
    public:
        static const int width = 8;
        static const int height = 8;
        static const int tileSize = 80;
        static void render(sf::RenderWindow& window, Game& game);
};

#endif