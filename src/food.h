//
// Created by sun on 2023/12/29.
//

#ifndef SNAKE_GAME_PROJECT_FOOD_H
#define SNAKE_GAME_PROJECT_FOOD_H


#include "SDL.h"
#include "snake.h"
#include <array>

class Food {
public:
    SDL_Point point{};

    Food() = default;

    explicit Food(SDL_Point p) : point(p) {};

    Food(int x, int y) : point{x, y} {};

    int life = 10 * 60;

    virtual std::array<int, 3> GetColors() const = 0;

    virtual void Consume(Snake &snake) const = 0;

    virtual ~Food() = default;
};


#endif //SNAKE_GAME_PROJECT_FOOD_H
