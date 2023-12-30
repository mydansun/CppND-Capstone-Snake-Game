#ifndef GAME_H
#define GAME_H

#include <random>
#include <vector>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "food.h"
#include "helpers.h"
#include "a_star.h"
#include "obstacle.h"
#include <memory>
#include <mutex>
#include <experimental/filesystem>

enum FoodType {
    Normal,
    Fast,
};

class Game {
public:
    Game(std::size_t grid_width, std::size_t grid_height);

    void Run(Controller const &controller, Renderer &renderer,
             std::size_t target_frame_duration);

    int GetScore() const;

    int GetSize() const;

private:
    std::string _save_path{std::experimental::filesystem::current_path() / "snake-game-records.txt"};

    std::mutex foods_mtx;
    std::size_t _grid_width;
    std::size_t _grid_height;
    Snake snake;
    Snake a_snake;
    AStar a_star;
    std::shared_ptr<Food> a_goal;
    std::vector<std::shared_ptr<Food>> foods;
    std::vector<Obstacle> obstacles;
    std::mutex obstacles_mtx;
    std::vector<int> random_obstacles_spawn{1, 2, 3};
    std::vector<double> random_obstacles_weights = {0.6, 0.3, 0.1};
    int max_obstacles_count = 5;
    std::uniform_int_distribution<int> random_obstacle_life{10 * 60, 30 * 60};
    const int obstacle_borders[12][2]{{-1, 0},
                                      {0,  -1},
                                      {1,  0},
                                      {0,  1},
                                      {-2, 0},
                                      {0,  -2},
                                      {2,  0},
                                      {0,  2},
                                      {-3, 0},
                                      {0,  -3},
                                      {3,  0},
                                      {0,  3}
    };

    std::random_device dev;
    std::mt19937 engine;
    std::uniform_int_distribution<int> random_w;
    std::uniform_int_distribution<int> random_h;
    std::vector<int> random_foods_spawn{1, 2};
    std::vector<double> random_foods_weights = {0.6, 0.4};
    int max_foods_count = 4;

    std::vector<FoodType> food_types = {FoodType::Normal, FoodType::Fast};
    std::vector<double> food_weights = {0.7, 0.3};

    int score{0};

    void PlaceFood();

    bool IsFreePlace(int x, int y) const;

    int CleanDeadFoods();

    void fillAStarObstacles();

    void SyncAStarGrid();

    bool ConsumeFood();

    void DriveASnake();

    void HandleASnake();

    void HandleSnake(const Controller &controller, bool &running);

    void HandleFoods();

    void PlaceObstacle();

    int CleanDeadObstacles();

    bool IsFreeWithObstacleBorders(int x, int y);

    bool CheckCollisionsWithObstacles(const Snake &s);

    void HandleObstacles();

    void SaveGameRecord(const std::string &player_name) const;
};

#endif