#pragma once

#include <cstddef>
#include <utility>

#include "ffca/Grid2D.hpp"

namespace ffca {

    template <typename Field, typename Rule>
    class Engine2D {
    public:
        explicit Engine2D(Grid2D<Field> initial_grid, Rule rule = Rule{})
            : current_(std::move(initial_grid)),
            next_(current_.width(), current_.height()),
            rule_(std::move(rule)) {}
        
        [[nodiscard]] Grid2D<Field>& current() noexcept {
            return current_;
        }

        [[nodiscard]] const Grid2D<Field>& current() const noexcept {
            return current_;
        }

        void step() {
            const std::size_t height = current_.height();
            const std::size_t width = current_.width();

            // For tiny grids, the boundary is basically the whole grid.
            // Use the simple wrapped version.
            if (height < 3 || width < 3) {
                step_wrapped_all();
                return;
            }

            // Interior cells: no bounds checks, no modulo wrapping.
            for (std::size_t row = 1; row + 1 < height; ++row) {
                for (std::size_t col = 1; col + 1 < width; ++col) {
                    const Field x = current_.unchecked(row, col);
                    const Field north = current_.unchecked(row - 1, col);
                    const Field south = current_.unchecked(row + 1, col);
                    const Field east = current_.unchecked(row, col + 1);
                    const Field west = current_.unchecked(row, col - 1);

                    next_.unchecked(row, col) = rule_(x, north, south, east, west);
                }
            }

            // Top and bottom rows.
            const std::size_t top = 0;
            const std::size_t bottom = height - 1;

            for (std::size_t col = 0; col < width; ++col) {
                compute_wrapped_cell(top, col);
                compute_wrapped_cell(bottom, col);
            }

            // Left and right columns, excluding corners already handled above.
            const std::size_t left = 0;
            const std::size_t right = width - 1;

            for (std::size_t row = 1; row + 1 < height; ++row) {
                compute_wrapped_cell(row, left);
                compute_wrapped_cell(row, right);
            }

            std::swap(current_, next_);
        }

    private:
        void step_wrapped_all() {
            for (std::size_t row = 0; row < current_.height(); ++row) {
                for (std::size_t col = 0; col < current_.width(); ++col) {
                    compute_wrapped_cell(row, col);
                }
            }

            std::swap(current_, next_);
        }

        void compute_wrapped_cell(std::size_t row, std::size_t col) {
            const int r = static_cast<int>(row);
            const int c = static_cast<int>(col);

            const Field x = current_.unchecked(row, col);
            const Field north = current_.wrapped(r - 1, c);
            const Field south = current_.wrapped(r + 1, c);
            const Field east = current_.wrapped(r, c + 1);
            const Field west = current_.wrapped(r, c - 1);

            next_.unchecked(row, col) = rule_(x, north, south, east, west);
        }

        Grid2D<Field> current_;
        Grid2D<Field> next_;
        Rule rule_;
    };
} // namespace ffca