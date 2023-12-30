#include "renderer.h"
#include "text.h"
#include "button.h"
#include <iostream>
#include <string>
#include <regex>

Renderer::Renderer(const std::size_t screen_width,
                   const std::size_t screen_height,
                   const std::size_t grid_width, const std::size_t grid_height)
        : screen_width(screen_width),
          screen_height(screen_height),
          grid_width(grid_width),
          grid_height(grid_height) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize.\n";
        std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
    }
    if (TTF_Init() == -1) {
        throw std::runtime_error("SDL_ttf could not initialize!");
    }
    font_large = TTF_OpenFont("assets/Roboto-Regular.ttf", 35);
    font_normal = TTF_OpenFont("assets/Roboto-Regular.ttf", 25);
    font_small = TTF_OpenFont("assets/Roboto-Regular.ttf", 15);
    font_title = TTF_OpenFont("assets/Silkscreen-Regular.ttf", 35);
    if (!font_large || !font_normal || !font_small || !font_title) {
        throw std::runtime_error("Unable to load fonts.");
    }
    // Create Window
    sdl_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, screen_width,
                                  screen_height, SDL_WINDOW_SHOWN);

    if (nullptr == sdl_window) {
        std::cerr << "Window could not be created.\n";
        std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
    }

    // Create renderer
    sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
    if (nullptr == sdl_renderer) {
        std::cerr << "Renderer could not be created.\n";
        std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
    }
}

Renderer::~Renderer() {
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
}

void Renderer::Render(Snake const &snake, Snake const &a_snake, const std::vector<std::shared_ptr<Food>> &foods,
                      const std::vector<Obstacle> &obstacles) {
    SDL_Rect block;
    block.w = screen_width / grid_width;
    block.h = screen_height / grid_height;

    // Clear screen
    SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
    SDL_RenderClear(sdl_renderer);

    // Render foods
    for (const auto &food: foods) {
        auto color = food->GetColors();
        SDL_SetRenderDrawColor(sdl_renderer, color[0], color[1], color[2], 0xFF);
//        block.x = food->point.x * block.w;
//        block.y = food->point.y * block.h;
//        SDL_RenderFillRect(sdl_renderer, &block);
        DrawCircle(sdl_renderer, food->point.x * block.w + block.w / 2, food->point.y * block.h + block.h / 2,
                   block.w / 2);
    }

    for (const auto &obstacle: obstacles) {
        SDL_SetRenderDrawColor(sdl_renderer, 0xA6, 0xA6, 0xA6, 0xFF);
        block.x = obstacle.point.x * block.w;
        block.y = obstacle.point.y * block.h;
        SDL_RenderFillRect(sdl_renderer, &block);
    }

    if (a_snake.alive) {
        // Render a_snake's body
        SDL_SetRenderDrawColor(sdl_renderer, 0xC9, 0xFF, 0xC9, 0xFF);
        for (SDL_Point const &point: a_snake.body) {
            block.x = point.x * block.w;
            block.y = point.y * block.h;
            SDL_RenderFillRect(sdl_renderer, &block);
        }
        // Render a_snake's head
        block.x = static_cast<int>(a_snake.head_x) * block.w;
        block.y = static_cast<int>(a_snake.head_y) * block.h;
        SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderFillRect(sdl_renderer, &block);
    }

    // Render snake's body
    SDL_SetRenderDrawColor(sdl_renderer, 0xC9, 0xC9, 0xFF, 0xFF);
    for (SDL_Point const &point: snake.body) {
        block.x = point.x * block.w;
        block.y = point.y * block.h;
        SDL_RenderFillRect(sdl_renderer, &block);
    }

    // Render snake's head
    block.x = static_cast<int>(snake.head_x) * block.w;
    block.y = static_cast<int>(snake.head_y) * block.h;
    if (snake.alive) {
        SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0x00, 0xFF, 0xFF);
    } else {
        SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
    }
    SDL_RenderFillRect(sdl_renderer, &block);

    // Update Screen
    SDL_RenderPresent(sdl_renderer);
}


bool Renderer::Welcome(bool &high_speed_start, std::string &player_name) const {
    SDL_Color white_color = {0xFF, 0xFF, 0xFF, 0xFF};
    Text welcome_text(font_title, sdl_renderer);
    welcome_text.build("Welcome to the Snake Game.", white_color);
    Text please_input_text(font_normal, sdl_renderer);
    please_input_text.build("Please input your name.", white_color);
    Text input_help_text(font_normal, sdl_renderer);
    input_help_text.build("(only supports alphanumeric characters)", white_color);


    Text normal_start_text(font_normal, sdl_renderer);
    normal_start_text.build("Start Game (normal speed)", white_color);
    Button normal_start_btn;

    Text high_start_text(font_normal, sdl_renderer);
    high_start_text.build("Start Game (high speed)", white_color);
    Button high_start_btn;

    std::string input_value;
    Text name_text(font_small, sdl_renderer);

    SDL_StartTextInput();

    SDL_Event e;
    while (true) {
        bool textNeedUpdate = false;

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                SDL_StopTextInput();
                return false;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_BACKSPACE && !input_value.empty()) {
                    input_value.pop_back();
                    textNeedUpdate = true;
                }
            } else if (e.type == SDL_TEXTINPUT) {
                input_value += e.text.text;
                textNeedUpdate = true;
            }
            normal_start_btn.HandleEvent(&e);
            high_start_btn.HandleEvent(&e);
        }

        if (normal_start_btn.GetState() == ButtonState::BUTTON_MOUSE_DOWN && ValidatePlayerName(input_value)) {
            high_speed_start = false;
            break;
        }

        if (high_start_btn.GetState() == ButtonState::BUTTON_MOUSE_DOWN && ValidatePlayerName(input_value)) {
            high_speed_start = true;
            break;
        }

        if (textNeedUpdate) {
            if (!input_value.empty()) {
                name_text.build(input_value, white_color);
            } else {
                name_text.build(" ", white_color);
            }
        }

        SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
        SDL_RenderClear(sdl_renderer);

        welcome_text.render(5, 0);
        please_input_text.render(5, welcome_text.getBottom() + 10);
        input_help_text.render(5, please_input_text.getBottom() + 10);
        name_text.render(5, input_help_text.getBottom() + 20);
        normal_start_text.render(5, 300);

        normal_start_btn.SetPosition(5, 300).SetSize(normal_start_text.getWidth(),
                                                     normal_start_text.getHeight());

        high_start_text.render(5, 350);
        high_start_btn.SetPosition(5, 350).SetSize(high_start_text.getWidth(), high_start_text.getHeight());

        //Update screen
        SDL_RenderPresent(sdl_renderer);
    }

    //Disable text input_value
    SDL_StopTextInput();
    if (input_value.empty()) {
        player_name = "player";
    } else {
        player_name = input_value;
    }
    return true;
}

void Renderer::RenderEndScreen(const std::string &save_path) {
    SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
    SDL_RenderClear(sdl_renderer);

    SDL_Color white_color = {0xFF, 0xFF, 0xFF, 0xFF};
    Text over_text(font_title, sdl_renderer);
    over_text.build("GAME OVER", white_color);
    over_text.render(5, 0);

    // Update Screen
    SDL_RenderPresent(sdl_renderer);
    if (!save_path_displayed) {
        save_path_displayed = true;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Alert",
                                 std::string("Your record was saved at: " + save_path).c_str(), sdl_window);
    }
}

bool Renderer::ValidatePlayerName(const std::string &input_value) const {
    std::regex reg(R"(^\w+$)");
    if (!std::regex_match(input_value, reg)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Alert",
                                 "The name is required and can only contain alphanumeric characters.", sdl_window);
        return false;
    }
    return true;
}

void Renderer::UpdateWindowTitle(const std::string &title) {
    SDL_SetWindowTitle(sdl_window, title.c_str());
}

void Renderer::DrawCircle(SDL_Renderer *renderer, int centre_x, int centre_y, int radius) const {
    int diameter = (radius * 2);

    int x = (radius - 1);
    int y = 0;
    int tx = 1;
    int ty = 1;
    int error = (tx - diameter);

    while (x >= y) {
        //  Each of the following renders an octant of the circle
        SDL_RenderDrawPoint(renderer, centre_x + x, centre_y - y);
        SDL_RenderDrawPoint(renderer, centre_x + x, centre_y + y);
        SDL_RenderDrawPoint(renderer, centre_x - x, centre_y - y);
        SDL_RenderDrawPoint(renderer, centre_x - x, centre_y + y);
        SDL_RenderDrawPoint(renderer, centre_x + y, centre_y - x);
        SDL_RenderDrawPoint(renderer, centre_x + y, centre_y + x);
        SDL_RenderDrawPoint(renderer, centre_x - y, centre_y - x);
        SDL_RenderDrawPoint(renderer, centre_x - y, centre_y + x);

        if (error <= 0) {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0) {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}
