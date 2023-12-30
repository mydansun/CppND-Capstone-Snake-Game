//
// Created by sun on 2023/12/29.
//

#include "fast_food.h"

void FastFood::Consume(Snake &snake) const {
    snake.speed += 0.02;
}

std::array<int, 3> FastFood::GetColors() const {
    return std::array<int, 3>{0x00, 0xFB, 0xFF};
}

