//
// Created by sun on 2024/1/2.
//

#include "text.h"

void Text::free() {
    if (_texture != nullptr) {
        SDL_DestroyTexture(_texture);
        _texture = nullptr;
    }
}

void Text::build(const std::string &content, SDL_Color color) {
    _content = content;
    _color = color;

    free();

    SDL_Surface *surface = TTF_RenderText_Solid(_font, _content.c_str(), _color);

    _texture = SDL_CreateTextureFromSurface(_sdl_renderer, surface);
    _width = surface->w;
    _height = surface->h;
    SDL_FreeSurface(surface);
}

Text::~Text() {
    free();
}

void Text::render(int x, int y) {
    _x = x;
    _y = y;
    SDL_Rect renderQuad = {x, y, _width, _height};
    SDL_RenderCopyEx(_sdl_renderer, _texture, nullptr, &renderQuad, 0.0, nullptr, SDL_FLIP_NONE);
}

Text::Text(const Text &text) {
    _font = text._font;
    _sdl_renderer = text._sdl_renderer;
    _content = text._content;
    _color = text._color;
    _x = text._x;
    _y = text._y;

    SDL_Surface *surface = TTF_RenderText_Solid(_font, _content.c_str(), _color);
    _texture = SDL_CreateTextureFromSurface(_sdl_renderer, surface);
    _width = surface->w;
    _height = surface->h;
    SDL_FreeSurface(surface);
}

Text &Text::operator=(const Text &text) {
    if (&text != this) {
        _font = text._font;
        _sdl_renderer = text._sdl_renderer;
        _content = text._content;
        _color = text._color;
        _x = text._x;
        _y = text._y;

        SDL_Surface *surface = TTF_RenderText_Solid(_font, _content.c_str(), _color);
        _texture = SDL_CreateTextureFromSurface(_sdl_renderer, surface);
        _width = surface->w;
        _height = surface->h;
        SDL_FreeSurface(surface);
    }
    return *this;
}

Text::Text(const Text &&text) noexcept {
    _font = text._font;
    _sdl_renderer = text._sdl_renderer;
    _content = text._content;
    _color = text._color;
    _x = text._x;
    _y = text._y;

    _texture = std::move(text._texture);
}

Text &Text::operator=(Text &&text) noexcept {
    if (&text != this) {
        _font = text._font;
        _sdl_renderer = text._sdl_renderer;
        _content = text._content;
        _color = text._color;
        _x = text._x;
        _y = text._y;

        _texture = std::move(text._texture);
    }
    return *this;
}
