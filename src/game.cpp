#include "game.h"
#include <future>
#include "SDL.h"
#include "normal_food.h"
#include "fast_food.h"
#include <algorithm>
#include <memory>
#include <fstream>

Game::Game(std::size_t grid_width, std::size_t grid_height)
        : _grid_width(grid_width),
          _grid_height(grid_height),
          snake(static_cast<int>(grid_width), static_cast<int>(grid_height)),
          a_snake(static_cast<int>(grid_width), static_cast<int>(grid_height)),
          engine(dev()),
          random_w(0, static_cast<int>(grid_width - 1)),
          random_h(0, static_cast<int>(grid_height - 1)) {

    a_snake.head_x = static_cast<float>(grid_width - 1);
    a_snake.head_y = static_cast<float>(grid_height - 1);


    PlaceObstacle();
    PlaceFood();

    a_star.initGrid(static_cast<int>(grid_width), static_cast<int>(grid_height));
    fillAStarObstacles();
}

void Game::PlaceObstacle() {
    int x, y;
    for (int i = 0; i < 20; ++i) {
        x = random_w(engine);
        y = random_h(engine);

        if (IsFreePlace(x, y) && IsFreeWithObstacleBorders(x, y)) {
            obstacles.emplace_back(x, y, random_obstacle_life(engine));
            break;
        }
    }
}

bool Game::IsFreeWithObstacleBorders(int x, int y) {
    for (auto b: obstacle_borders) {
        if (snake.SnakeCell(x + b[0], y + b[1])) {
            return false;
        }
        if (a_snake.SnakeCell(x + b[0], y + b[1])) {
            return false;
        }
    }
    return true;
}

void Game::fillAStarObstacles() {
//    a_star.fillObstacle(GridPoint{static_cast<int>(snake.head_x), static_cast<int>(snake.head_y)});
//    for (const auto &body: snake.body) {
//        a_star.fillObstacle(GridPoint{body.x, body.y});
//    }
    for (const auto &obstacle: obstacles) {
        a_star.fillObstacle(GridPoint{obstacle.point.x, obstacle.point.y});
    }
    for (const auto &a_body: a_snake.body) {
        a_star.fillObstacle(GridPoint{a_body.x, a_body.y});
    }
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
    Uint32 title_timestamp = SDL_GetTicks();
    Uint32 frame_start;
    Uint32 frame_end;
    Uint32 frame_duration;

    int frame_count = 0;
    bool running = true;
    std::future<void> sync_task;

    bool high_speed_start;
    std::string player_name;
    renderer.UpdateWindowTitle("Welcome to the Snake Game");
    if (!renderer.Welcome(high_speed_start, player_name)) {
        running = false;
    } else {
        snake.speed = high_speed_start ? 0.5f : snake.speed;
    }

    bool saved_record = false;

    while (running) {
        frame_start = SDL_GetTicks();
        if (sync_task.valid()) {
            sync_task.wait();
        }

        auto a_snake_task = std::async(std::launch::async, [this] { HandleASnake(); });
        HandleSnake(controller, running);
        a_snake_task.wait();

        if (!snake.alive) {
            if (!saved_record) {
                saved_record = true;
                SaveGameRecord(player_name);
            }
            renderer.RenderEndScreen(_save_path);
        } else {
            HandleObstacles();
            HandleFoods();
            sync_task = std::async(std::launch::async, [this] { SyncAStarGrid(); });
            renderer.Render(snake, a_snake, foods, obstacles);
        }

        frame_end = SDL_GetTicks();

        // Keep track of how long each loop through the input/update/render cycle
        // takes.
        frame_count++;
        frame_duration = frame_end - frame_start;

        // After every second, update the window title.
        if (frame_end - title_timestamp >= 1000) {
            renderer.UpdateWindowTitle(
                    "Name: " + player_name + " Score: " + std::to_string(score) + " FPS: " +
                    std::to_string(frame_count));
            frame_count = 0;
            title_timestamp = frame_end;
        }

        // If the time for this frame is too small (i.e. frame_duration is
        // smaller than the target ms_per_frame), delay the loop to
        // achieve the correct frame rate.
        if (frame_duration < target_frame_duration) {
            SDL_Delay(target_frame_duration - frame_duration);
        }
    }
}

void Game::SaveGameRecord(const std::string &player_name) const {
    std::fstream file;
    file.open(_save_path, std::ios::out | std::ios::app);
    file << "Player: " << player_name << ", Score: " << score << ".\n";
    file.close();
}

void Game::HandleObstacles() {
    std::lock_guard<std::mutex> lck(obstacles_mtx);
    if (CleanDeadObstacles()) {
        if (obstacles.size() < max_obstacles_count) {
            auto new_obstacles_count = random_choice(engine, random_obstacles_spawn, random_obstacles_weights);
            for (int i = 0; i < new_obstacles_count; ++i) {
                PlaceObstacle();
                if (foods.size() >= max_obstacles_count) {
                    break;
                }
            }
        }
    }
}

int Game::CleanDeadObstacles() {
    int cleaned_obstacles_count = 0;
    for (auto it = obstacles.begin(); it != obstacles.end();) {
        (*it).life--;
        if ((*it).life <= 0) {
            it = obstacles.erase(it);
            cleaned_obstacles_count++;
        } else {
            ++it;
        }
    }
    return cleaned_obstacles_count;
}

void Game::HandleFoods() {
    std::lock_guard<std::mutex> lck(foods_mtx);
    if (ConsumeFood()) {
        CleanDeadFoods();
        if (foods.size() < max_foods_count) {
            auto new_foods_count = random_choice(engine, random_foods_spawn, random_foods_weights);
            for (int i = 0; i < new_foods_count; ++i) {
                PlaceFood();
                if (foods.size() >= max_foods_count) {
                    break;
                }
            }
        }
    }
}

void Game::HandleSnake(const Controller &controller, bool &running) {
    controller.HandleInput(running, snake);
    if (snake.alive) {
        snake.Update();
        if (CheckCollisionsWithObstacles(snake)) {
            snake.alive = false;
        }
    }
}

bool Game::CheckCollisionsWithObstacles(const Snake &s) {
    std::lock_guard<std::mutex> lck(obstacles_mtx);
    if (std::any_of(obstacles.cbegin(), obstacles.cend(), [s](const auto &obstacle) {
        return obstacle.point.x == static_cast<int>(s.head_x) && obstacle.point.y == static_cast<int>(s.head_y);
    })) {
        return true;
    }

    for (const auto item: s.body) {
        if (std::any_of(obstacles.cbegin(), obstacles.cend(), [item](const auto &obstacle) {
            return obstacle.point.x == item.x && obstacle.point.y == item.y;
        })) {
            return true;
        }
    }
    return false;
}

void Game::HandleASnake() {
    if (a_snake.alive) {
        DriveASnake();
        a_snake.Update();
        if (CheckCollisionsWithObstacles(a_snake)) {
            a_snake.alive = false;
        }
    }

    if (!a_snake.alive && IsFreePlace(static_cast<int>(_grid_width - 1), static_cast<int>(_grid_height - 1))) {
        a_snake = Snake(static_cast<int>(_grid_width), static_cast<int>(_grid_height));
        a_snake.head_x = static_cast<float>(_grid_width - 1);
        a_snake.head_y = static_cast<float>(_grid_height - 1);
    }
}

void Game::DriveASnake() {
    std::lock_guard<std::mutex> lck(foods_mtx);
    if (!a_goal || a_goal->life == 0) {
        if (foods.empty()) {
            a_goal = nullptr;
        } else {
            a_goal = *select_randomly(foods.rbegin(), foods.rend(), engine);
        }
    }
    if (a_goal && a_goal->life > 0) {
        auto direction = a_star.Search({static_cast<int>(a_snake.head_x), static_cast<int>(a_snake.head_y)},
                                       {a_goal->point.x, a_goal->point.y});
        if (direction[0] == 1 && direction[1] == 0) {
            a_snake.ChangeDirection(Snake::Direction::kRight,
                                    Snake::Direction::kLeft);
        } else if (direction[0] == -1 && direction[1] == 0) {
            a_snake.ChangeDirection(Snake::Direction::kLeft,
                                    Snake::Direction::kRight);
        } else if (direction[0] == 0 && direction[1] == 1) {
            a_snake.ChangeDirection(Snake::Direction::kDown,
                                    Snake::Direction::kUp);
        } else if (direction[0] == 0 && direction[1] == -1) {
            a_snake.ChangeDirection(Snake::Direction::kUp,
                                    Snake::Direction::kDown);
        }
    }
}

void Game::SyncAStarGrid() {
    a_star.resetGrid();
    fillAStarObstacles();
}

void Game::PlaceFood() {
    int x, y;
    while (true) {
        x = random_w(engine);
        y = random_h(engine);

        if (IsFreePlace(x, y)) {
            auto chosen_type = random_choice(engine, food_types, food_weights);
            if (chosen_type == FoodType::Normal) {
                foods.emplace_back(std::make_shared<NormalFood>(x, y));
                return;
            } else if (chosen_type == FoodType::Fast) {
                foods.emplace_back(std::make_shared<FastFood>(x, y));
                return;
            } else {
                throw std::invalid_argument("Unknown food type");
            }
        }
    }
}

bool Game::IsFreePlace(const int x, const int y) const {
    if (snake.SnakeCell(x, y)) {
        return false;
    }
    if (a_snake.SnakeCell(x, y)) {
        return false;
    }
    if (std::any_of(foods.cbegin(), foods.cend(), [x, y](const auto &food) {
        return food->point.x == x && food->point.y == y;
    })) {
        return false;
    }
    if (std::any_of(obstacles.cbegin(), obstacles.cend(), [x, y](const auto &obstacle) {
        return obstacle.point.x == x && obstacle.point.y == y;
    })) {
        return false;
    }
    return true;
}

bool Game::ConsumeFood() {
    int new_x = static_cast<int>(snake.head_x);
    int new_y = static_cast<int>(snake.head_y);

    int new_a_x = static_cast<int>(a_snake.head_x);
    int new_a_y = static_cast<int>(a_snake.head_y);

    bool has_dead_food = false;
    for (auto &food: foods) {
        if (food->point.x == new_x && food->point.y == new_y && food->life > 0) {
            food->Consume(snake);
            score++;
            food->life = 0;
        }
        if (food->point.x == new_a_x && food->point.y == new_a_y && food->life > 0) {
            food->Consume(a_snake);
            food->life = 0;
        }
        if (food->life > 0) {
            food->life--;
        }
        if (food->life == 0 && !has_dead_food) {
            has_dead_food = true;
        }
    }
    return has_dead_food;
}

int Game::CleanDeadFoods() {
    int cleaned_foods_count = 0;
    for (auto it = foods.begin(); it != foods.end();) {
        if ((*it)->life == 0) {
            it = foods.erase(it);
            cleaned_foods_count++;
        } else {
            ++it;
        }
    }
    return cleaned_foods_count;
}

int Game::GetScore() const { return score; }

int Game::GetSize() const { return snake.size; }