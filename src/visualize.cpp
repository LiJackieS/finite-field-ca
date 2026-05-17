#include <SDL.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <utility>

#include "ffca/Engine.hpp"
#include "ffca/Field.hpp"
#include "ffca/Grid2D.hpp"
#include "ffca/Rules.hpp"
#include "ffca/Utils.hpp"

namespace {

struct Color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

constexpr std::array<Color, 5> colors = {{
    {10, 10, 20},     // 0
    {70, 130, 180},   // 1
    {120, 200, 120},  // 2
    {240, 180, 80},   // 3
    {220, 80, 100}    // 4
}};

} // namespace

int main() {
    using F5 = ffca::Fp<5>;

    constexpr std::size_t grid_width = 160;
    constexpr std::size_t grid_height = 120;
    constexpr int cell_size = 5;
    constexpr std::uint32_t seed = 42;

    constexpr int window_width = static_cast<int>(grid_width) * cell_size;
    constexpr int window_height = static_cast<int>(grid_height) * cell_size;

    auto make_engine = [] {
        ffca::Grid2D<F5> grid{grid_width, grid_height};

        // Random initialization
        ffca::randomize_grid(grid, seed);

        // Rule: x' = x^2 + N + S + E + W mod 5
        return ffca::Engine2D<F5, ffca::QuadraticVonNeumannRule<F5>>{
            std::move(grid)
        };
    };

    auto engine = make_engine();

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Finite-Field Cellular Automata",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    bool automatic = true;
    bool step_once = false;

    SDL_Event event{};

    std::cout << "Controls:\n";
    std::cout << "  Space       toggle automatic/manual mode\n";
    std::cout << "  Right arrow step once in manual mode\n";
    std::cout << "  R           reset grid\n";
    std::cout << "  Esc         quit\n";

    while (running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = false;
                        break;

                    case SDLK_SPACE:
                        automatic = !automatic;
                        std::cout << (automatic ? "Automatic mode\n" : "Manual mode\n");
                        break;

                    case SDLK_RIGHT:
                        step_once = true;
                        break;

                    case SDLK_r:
                        engine = make_engine();
                        std::cout << "Reset grid\n";
                        break;

                    default:
                        break;
                }
            }
        }

        if (automatic || step_once) {
            engine.step();
            step_once = false;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        const auto& current = engine.current();

        for (std::size_t row = 0; row < current.height(); ++row) {
            for (std::size_t col = 0; col < current.width(); ++col) {
                const auto value = current(row, col).value();
                const Color color = colors[value];

                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

                SDL_Rect rect{
                    static_cast<int>(col) * cell_size,
                    static_cast<int>(row) * cell_size,
                    cell_size,
                    cell_size
                };

                SDL_RenderFillRect(renderer, &rect);
            }
        }

        SDL_RenderPresent(renderer);

        // Controls speed in automatic mode and avoids maxing CPU in manual mode.
        SDL_Delay(30);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}