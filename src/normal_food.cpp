//
// Created by sun on 2023/12/29.
//

#include "normal_food.h"

void NormalFood::Consume(Snake &snake) const {
    snake.GrowBody();
}

std::array<int, 3> NormalFood::GetColors() const {
    return std::array<int, 3>{0xFF, 0xCC, 0x00};
}
