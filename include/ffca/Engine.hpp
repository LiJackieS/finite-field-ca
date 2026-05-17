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
            for (std::size_t row = 0; row < current_.height(); ++row) {
                for (std::size_t col = 0; col < current_.width(); ++col) {
                    int r = static_cast<int>(row);
                    int c = static_cast<int>(col);

                    Field x = current_(row, col);

                    Field north = current_.wrapped(r - 1, c);
                    Field south = current_.wrapped(r + 1, c);
                    Field east = current_.wrapped(r, c + 1);
                    Field west = current_.wrapped(r, c - 1);

                    next_(row, col) = rule_(x, north, south, east, west);
                }
            }

            std::swap(current_, next_);
        }
    private:
        Grid2D<Field> current_;
        Grid2D<Field> next_;
        Rule rule_;
    };
} // namespace ffca