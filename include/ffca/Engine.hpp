#pragma once

#include <algorithm>
#include <cstddef>
#include <thread>
#include <utility>
#include <vector>

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

            if (height < 3 || width < 3) {
                step_wrapped_all();
                return;
            }

            step_interior_range(1, height - 1);
            compute_boundary_cells();

            std::swap(current_, next_);
        }

        void step_parallel(std::size_t thread_count) {
            const std::size_t height = current_.height();
            const std::size_t width = current_.width();
            
            if (height < 3 || width < 3 || thread_count <= 1) {
                step();
                return;
            }
            
            const std::size_t interior_rows = height - 2;

            thread_count = std::min(interior_rows, thread_count);

            {
                std::vector<std::jthread> workers;
                workers.reserve(thread_count);

                const std::size_t base_rows = interior_rows / thread_count;
                const std::size_t extra_rows = interior_rows % thread_count;

                std::size_t row_begin = 1;

                for (std::size_t t = 0; t < thread_count; ++t) {
                    const std::size_t rows_for_thread = base_rows + (t < extra_rows ? 1 : 0);
                    const std::size_t row_end = row_begin + rows_for_thread;

                    workers.emplace_back([this, row_begin, row_end] {
                        step_interior_range(row_begin, row_end);
                    });

                    row_begin = row_end;
                }
            } // std::jthread destructors auto-join here

            compute_boundary_cells();
            std::swap(current_, next_);
        }

    private:
        void step_interior_range(std::size_t row_begin, std::size_t row_end) {
            const std::size_t width = current_.width();

            for (std::size_t row = row_begin; row < row_end; ++row) {
                for (std::size_t col = 1; col + 1 < width; ++col) {
                    const Field x = current_.unchecked(row, col);
                    const Field north = current_.unchecked(row - 1, col);
                    const Field south = current_.unchecked(row + 1, col);
                    const Field east = current_.unchecked(row, col + 1);
                    const Field west = current_.unchecked(row, col - 1);

                    next_.unchecked(row, col) = rule_(x, north, south, east, west);
                }
            }
        }

        void compute_boundary_cells() {
            const std::size_t height = current_.height();
            const std::size_t width = current_.width();

            const std::size_t top = 0;
            const std::size_t bottom = height - 1;

            for (std::size_t col = 0; col < width; ++col) {
                compute_wrapped_cell(top, col);
                compute_wrapped_cell(bottom, col);
            }

            const std::size_t left = 0;
            const std::size_t right = width - 1;

            for (std::size_t row = 1; row + 1 < height; ++row) {
                compute_wrapped_cell(row, left);
                compute_wrapped_cell(row, right);
            }
        }
        
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