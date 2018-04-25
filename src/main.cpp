
#include "../bin/version.h"

#include <iostream>
#include <boost/program_options.hpp>
#include <spdlog/spdlog.h>

#include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>

#include <cmath>

void main_body();

int main(int argc, char** argv)
{
    auto console = spdlog::stdout_logger_st("console");
    console->info("Wellcome!");

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
    ("help,h", "print usage message")
    ("version,v", "print version number");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
    } else if (vm.count("version")) {
        std::cout << "Build version: " << build_version() << std::endl;
        std::cout << "Boost version: " << (BOOST_VERSION / 100000) << '.' << (BOOST_VERSION / 100 % 1000) << '.' << (BOOST_VERSION % 100) << std::endl;
    } else {
        main_body();
    }

    console->info("Goodby!");

    return 0;
}

void main_body()
{
    auto console = spdlog::get("console");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        console->error("SDL_Init Error: {0}", SDL_GetError());
        throw std::runtime_error("SDL_Init");
    }

    SDL_DisplayMode display_mode;
    if (SDL_GetCurrentDisplayMode(0, &display_mode) != 0) {
        console->error("SDL_GetCurrentDisplayMode Error: {0}", SDL_GetError());
        SDL_Quit();
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

    bool run = true;

    auto start = std::chrono::system_clock::now();
    size_t count = 0;
    auto last = start;
    size_t last_count = count;
    double time_step = 0.0;

    while (run) {
        auto loop_start = std::chrono::system_clock::now();
        ++count;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Rect rect;
        rect.x = 5;
        rect.y = 5;
        rect.w = width - 10;
        rect.h = height - 10;
        SDL_SetRenderDrawColor(renderer, 0x7f, 0x7f, 0x7f, 255);
        SDL_RenderDrawRect(renderer, &rect);

        size_t N = 20;
        double sx = double(width - 10) / N;
        double sy = double(height - 10) / N;

        for(size_t n = 0; n < 5; ++n) {
            SDL_SetRenderDrawColor(renderer, 0x7f + n * 0x10, 0x7f + n * 0x10, 0x7f + n * 0x10, 255);
            size_t m = size_t(n + count * 10 * time_step) % N;
            SDL_RenderDrawLine(renderer, 5, 5 + m * sy, 5 + m * sx, height - 5);
            SDL_RenderDrawLine(renderer, 5 + (N - m) * sx, 5, width - 5, 5 + (N - m) * sy);

            SDL_RenderDrawLine(renderer, 5, 5 + m * sy, 5 + (N - m) * sx, 5);
            SDL_RenderDrawLine(renderer, 5 + m * sx, height - 5, width - 5, 5 + (N - m) * sy);
        }

        size_t x = (sin(M_PI * count * time_step) + 2.0) * (width >> 2);
        size_t y = (cos(M_PI * count * time_step) + 2.0) * (height >> 2);
        SDL_RenderDrawLine(renderer, 5, y, width - 5, y);
        SDL_RenderDrawLine(renderer, x, 5, x, height - 5);

        SDL_RenderPresent(renderer);

        // SDL_LockSurface(img);
        // SDL_memset((unsigned char*)(img->pixels), 0xff, img->h * img->pitch);
        // for(size_t y = 0; y < img->h; ++y)
        //     for(size_t x = 0; x < img->w; ++x) {
        //         unsigned char* pixel = (unsigned char*)(img->pixels) + x * 4 + y * img->pitch;
        //         Uint8 lumr = x + count;
        //         Uint8 lumg = y + count;
        //         Uint8 lumb = x + y + count;
        //         *((Uint32*)pixel) = SDL_MapRGBA(img->format, lumr, lumg, lumb, 0x00);
        //     }
        // SDL_UnlockSurface(img);
        // SDL_BlitSurface(img, nullptr, scr, nullptr);

        // SDL_UpdateWindowSurface(win);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT
                || event.type == SDL_KEYDOWN
                || event.type == SDL_KEYUP) {
                run = false;
            }
        }

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> full_elapsed = end - start;
        std::chrono::duration<double> last_elapsed = end - last;
        std::chrono::duration<double> loop_elapsed = end - loop_start;
        time_step = (time_step * (count-1) + loop_elapsed.count()) / count;

        if (!run || last_elapsed.count() >= 1) {
            int frames = count - last_count;
            double fps = ((double)frames) / last_elapsed.count();

            SDL_SetWindowTitle(win, ("Hello World! FPS: " + std::to_string(fps)).c_str());

            console->info("[{0} / {1}] fps: {2}; time_step: {3}", full_elapsed.count(), count, fps, time_step);

            last = end;
            last_count = count;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
}