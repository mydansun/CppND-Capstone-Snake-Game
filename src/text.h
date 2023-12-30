//
// Created by sun on 2024/1/2.
//

#ifndef SNAKE_GAME_PROJECT_TEXT_H
#define SNAKE_GAME_PROJECT_TEXT_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class Text {
public:
    Text() = default;

    ~Text();

    Text(const Text &text);

    Text &operator=(const Text &text);

    Text(const Text &&text) noexcept;

    Text &operator=(Text &&text) noexcept;

    Text(TTF_Font *font, SDL_Renderer *sdl_renderer) : _font(font), _sdl_renderer(sdl_renderer) {};

    void free();

    void build(const std::string &content, SDL_Color color);

    int getWidth() { return _width; };

    int getHeight() { return _height; };

    int getBottom() { return _height + _y; };

    void render(int x, int y);

private:
    TTF_Font *_font = nullptr;
    SDL_Renderer *_sdl_renderer = nullptr;
    SDL_Texture *_texture = nullptr;
    int _width = 0;
    int _height = 0;
    int _x = 0;
    int _y = 0;
    std::string _content;
    SDL_Color _color{0, 0, 0, 0};
};


#endif //SNAKE_GAME_PROJECT_TEXT_H
