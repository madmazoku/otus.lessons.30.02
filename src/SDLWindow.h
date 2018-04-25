#pragma once

#include <spdlog/spdlog.h>
#include <SDL2/SDL.h>

class SDLWindow {
private:
public:
    SDLWindow() noexcept {};
    SDLWindow(const SDLWindow&) = delete;
    SDLWindow(SDLWindow&&) = delete;

    SDLWindow& operator=(const SDLWindow&) = delete;
    SDLWindow& operator=(SDLWindow&&) = delete;

    void create(const std::string& title = "SDLWindow", size_t  = 0, size_t h = 0, size_t x = SDL_WINDOWPOS_CENTERED, size_t y = SDL_WINDOWPOS_CENTERED) {
        auto console = spdlog::get("console");

        SDL_DisplayMode display_mode;
        if (SDL_GetCurrentDisplayMode(0, &display_mode) != 0) {
            console->error("SDL_GetCurrentDisplayMode Error: {0}", SDL_GetError());
            throw std::runtime_error("SDL_GetCurrentDisplayMode");
        }

        long width = display_mode.w >> 1;
        long height = display_mode.h >> 1;

        SDL_Window *win = SDL_CreateWindow(
                              "Hellow World!",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width,
                              height,
                              SDL_WINDOW_SHOWN
                          );
        if (win == nullptr) {
            console->error("SDL_CreateWindow Error: {0}", SDL_GetError());
            SDL_Quit();
            throw std::runtime_error("SDL_CreateWindow");
        }

        SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
        if (win == nullptr) {
            console->error("SDL_CreateRenderer Error: {0}", SDL_GetError());
            SDL_Quit();
            throw std::runtime_error("SDL_CreateRenderer");
        }

        SDL_Surface *scr = SDL_GetWindowSurface(win);
        SDL_Surface *img = SDL_CreateRGBSurface(0, scr->w, scr->h, 32, 0, 0, 0, 0);
    }

    static void lib_init() {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            spdlog::get("console")->error("SDL_Init Error: {0}", SDL_GetError());
            throw std::runtime_error("SDL_Init");
        }
    }

    static void lib_done() {
        SDL_Quit();
    }
};