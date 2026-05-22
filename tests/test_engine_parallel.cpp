#include <cassert>
#include <cstddef>
#include <iostream>
#include <utility>

#include "ffca/Engine.hpp"
#include "ffca/Field.hpp"
#include "ffca/Grid2D.hpp"
#include "ffca/Rules.hpp"
#include "ffca/Utils.hpp"

int main() {
    using F5 = ffca::Fp<5>;
    using Rule = ffca::QuadraticVonNeumannRule<F5>;
    using Engine = ffca::Engine2D<F5, Rule>;

    constexpr std::size_t width = 64;
    constexpr std::size_t height = 64;
    constexpr std::uint32_t seed = 42;

    ffca::Grid2D<F5> grid_scalar{width, height};
    ffca::Grid2D<F5> grid_parallel{width, height};

    ffca::randomize_grid(grid_scalar, seed);
    ffca::randomize_grid(grid_parallel, seed);

    Engine scalar_engine{std::move(grid_scalar)};
    Engine parallel_engine{std::move(grid_parallel)};

    for (int step = 0; step < 20; ++step) {
        scalar_engine.step();
        parallel_engine.step_parallel(4);
    }

    const auto& scalar = scalar_engine.current();
    const auto& parallel = parallel_engine.current();

    for (std::size_t row = 0; row < height; ++row) {
        for (std::size_t col = 0; col < width; ++col) {
            assert(scalar(row, col) == parallel(row, col));
        }
    }

    std::cout << "Parallel engine tests passed.\n";
}