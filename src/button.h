//
// Created by sun on 2024/1/2.
//

#ifndef SNAKE_GAME_PROJECT_BUTTON_H
#define SNAKE_GAME_PROJECT_BUTTON_H

#include <SDL.h>

enum ButtonState {
    BUTTON_MOUSE_OUT = 0,
    BUTTON_MOUSE_OVER = 1,
    BUTTON_MOUSE_DOWN = 2,
    BUTTON_MOUSE_UP = 3,
};

class Button {
public:
    Button() = default;

    Button(const SDL_Point &position, int width, int height) : _position(position), _width(width), _height(height) {};

    void HandleEvent(SDL_Event *e);

    Button &SetPosition(int x, int y);

    Button &SetSize(int width, int height);

    ButtonState GetState() const { return _state; };

private:
    SDL_Point _position{0, 0};
    int _width = 0;
    int _height = 0;
    ButtonState _state = ButtonState::BUTTON_MOUSE_OUT;

    bool CheckBounding(int x, int y) const;
};


#endif //SNAKE_GAME_PROJECT_BUTTON_H
