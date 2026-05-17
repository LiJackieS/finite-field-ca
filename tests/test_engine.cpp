#include <cassert>
#include <iostream>

#include "ffca/Engine.hpp"
#include "ffca/Field.hpp"
#include "ffca/Grid2D.hpp"
#include "ffca/Rules.hpp"

int main() {
    using F5 = ffca::Fp<5>;

    ffca::Grid2D<F5> grid{3, 3};

    for (std::size_t row = 0; row < grid.height(); ++row) {
        for (std::size_t col = 0; col < grid.width(); ++col) {
            grid(row, col) = F5{1};
        }
    }

    ffca::Engine2D<F5, ffca::QuadraticVonNeumannRule<F5>> engine{
        std::move(grid)
    };

    engine.step();

    const auto& result = engine.current();

    for (std::size_t row = 0; row < result.height(); ++row) {
        for (std::size_t col = 0; col < result.width(); ++col) {
            assert(result(row, col) == F5{0});
        }
    }

    std::cout << "Engine tests passed.\n";
}