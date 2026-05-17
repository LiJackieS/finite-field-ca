#include <cstddef>
#include <iostream>

#include "ffca/Engine.hpp"
#include "ffca/Field.hpp"
#include "ffca/Grid2D.hpp"
#include "ffca/Rules.hpp"
#include "ffca/Utils.hpp"

int main() {
    using F5 = ffca::Fp<5>;

    constexpr std::size_t width = 16;
    constexpr std::size_t height = 16;

    ffca::Grid2D<F5> grid{width, height};

    ffca::randomize_grid(grid, 42);

    ffca::Engine2D<F5, ffca::QuadraticVonNeumannRule<F5>> engine{
        std::move(grid)
    };

    std::cout << "Initial grid:\n";
    ffca::print_grid(engine.current());

    for (int step = 1; step <= 10; ++step) {
        engine.step();

        std::cout << "\nAfter step " << step << ":\n";
        ffca::print_grid(engine.current());
    }

    return 0;
}