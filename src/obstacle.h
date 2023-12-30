//
// Created by sun on 2024/1/2.
//

#ifndef SNAKE_GAME_PROJECT_OBSTACLE_H
#define SNAKE_GAME_PROJECT_OBSTACLE_H

#include "SDL.h"

class Obstacle {
public:
    SDL_Point point{};

    Obstacle() = default;

    explicit Obstacle(SDL_Point p) : point(p) {};

    Obstacle(int x, int y) : point{x, y} {};

    Obstacle(int x, int y, int l) : point{x, y}, life(l) {};

    int life = 420;
};


#endif //SNAKE_GAME_PROJECT_OBSTACLE_H
