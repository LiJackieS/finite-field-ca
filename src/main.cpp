#include <cstddef>
#include <iostream>

#include "ffca/Engine.hpp"
#include "ffca/Field.hpp"
#include "ffca/Grid2D.hpp"
#include "ffca/Rules.hpp"
#include "ffca/Utils.hpp"

int main() {
    using F5 = ffca::Fp<5>;

    constexpr std::size_t width = 8;
    constexpr std::size_t height = 8;

    ffca::Grid2D<F5> grid{width, height};

    for (std::size_t row = 0; row < height; ++row) {
        for (std::size_t col = 0; col < width; ++col) {
            grid(row, col) = F5{row + col};
        }
    }

    ffca::Engine2D<F5, ffca::QuadraticVonNeumannRule<F5>> engine{
        std::move(grid)
    };

    std::cout << "Initial grid:\n";
    ffca::print_grid(engine.current());

    for (int step = 1; step <= 5; ++step) {
        engine.step();

        std::cout << "\nAfter step " << step << ":\n";
        ffca::print_grid(engine.current());
    }

    return 0;
}