#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>

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

    template <typename Field>
    void randomize_grid(Grid2D<Field>& grid, std::uint32_t seed = std::random_device{}()) {
        std::mt19937 rng(seed);

        std::uniform_int_distribution<std::uint32_t> dist(
            0,
            Field::modulus() - 1
        );

        for (std::size_t row = 0; row < grid.height(); ++row) {
            for (std::size_t col = 0; col < grid.width(); ++col) {
                grid(row, col) = Field{dist(rng)};
            }
        }
    }
} // namespace ffca