//
// Created by sun on 2023/12/29.
//

#ifndef SNAKE_GAME_PROJECT_HELPERS_H
#define SNAKE_GAME_PROJECT_HELPERS_H

#include <vector>
#include <numeric>
#include <random>
#include <iterator>

template<typename T, typename Engine>
T random_choice(Engine &generator, const std::vector<T> &sequence, const std::vector<double> &weights) {
    if (sequence.size() != weights.size() || sequence.empty()) {
        return {};
    }
    std::discrete_distribution<> dis(weights.begin(), weights.end());
    int index = dis(generator);
    return sequence.at(index);
}

template<typename Iter, typename Engine>
Iter select_randomly(Iter start, Iter end, Engine &generator) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(generator));
    return start;
}

#endif //SNAKE_GAME_PROJECT_HELPERS_H
