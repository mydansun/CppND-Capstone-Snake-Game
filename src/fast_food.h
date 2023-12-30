//
// Created by sun on 2023/12/29.
//

#ifndef SNAKE_GAME_PROJECT_FAST_FOOD_H
#define SNAKE_GAME_PROJECT_FAST_FOOD_H


#include "food.h"

class FastFood : public Food {
    using Food::Food;

    void Consume(Snake &snake) const override;

    std::array<int, 3> GetColors() const override;
};


#endif //SNAKE_GAME_PROJECT_FAST_FOOD_H
