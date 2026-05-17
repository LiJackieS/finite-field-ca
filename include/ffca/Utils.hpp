#pragma once

#include <cstddef>
#include <iostream>

#include "ffca/Grid2D.hpp"

namespace ffca {
    template <typename Field>
    void print_grid(const Grid2D<Field>& grid) {
        for (std::size_t row = 0; row < grid.height(); ++row) {
            for (std::size_t col = 0; col < grid.width(); ++col) {
                std::cout << grid(row, col) << ' ';
            }
            std::cout << '\n';
        }
    }
} // namespace ffca