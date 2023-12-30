#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include "SDL.h"
#include "snake.h"
#include "food.h"
#include <memory>
#include "SDL_ttf.h"
#include "obstacle.h"

class Renderer {
public:
    Renderer(const std::size_t screen_width, const std::size_t screen_height,
             const std::size_t grid_width, const std::size_t grid_height);

    ~Renderer();

    void Render(Snake const &snake, Snake const &a_snake, const std::vector<std::shared_ptr<Food>> &foods,
                const std::vector<Obstacle> &obstacles);

    void UpdateWindowTitle(const std::string &title);

    bool Welcome(bool &high_speed_start, std::string &player_name) const;

    void RenderEndScreen(const std::string &save_path);

private:
    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    TTF_Font *font_large;
    TTF_Font *font_normal;
    TTF_Font *font_small;
    TTF_Font *font_title;
    const std::size_t screen_width;
    const std::size_t screen_height;
    const std::size_t grid_width;
    const std::size_t grid_height;
    bool save_path_displayed = false;

    void DrawCircle(SDL_Renderer *renderer, int centre_x, int centre_y, int radius) const;

    bool ValidatePlayerName(const std::string &input_value) const;
};

#endif