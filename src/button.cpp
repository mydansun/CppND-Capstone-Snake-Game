//
// Created by sun on 2024/1/2.
//

#include "button.h"

void Button::HandleEvent(SDL_Event *e) {
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP) {
        int x, y;
        SDL_GetMouseState(&x, &y);

        bool is_inside = CheckBounding(x, y);

        if (!is_inside) {
            _state = BUTTON_MOUSE_OUT;
        } else {
            switch (e->type) {
                case SDL_MOUSEMOTION:
                    _state = ButtonState::BUTTON_MOUSE_OVER;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    _state = ButtonState::BUTTON_MOUSE_DOWN;
                    break;

                case SDL_MOUSEBUTTONUP:
                    _state = ButtonState::BUTTON_MOUSE_UP;
                    break;
                default:
                    _state = ButtonState::BUTTON_MOUSE_OUT;
                    break;
            }
        }
    }
}

bool Button::CheckBounding(int x, int y) const {
    if (x < _position.x) {
        return false;
    } else if (x > _position.x + _width) {
        return false;
    } else if (y < _position.y) {
        return false;
    } else if (y > _position.y + _height) {
        return false;
    }
    return true;
}

Button &Button::SetSize(int width, int height) {
    _width = width;
    _height = height;
    return *this;
}

Button &Button::SetPosition(int x, int y) {
    _position.x = x;
    _position.y = y;
    return *this;
}
