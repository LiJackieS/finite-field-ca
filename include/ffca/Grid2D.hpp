#pragma once

#include <cstddef>
#include <vector>
#include <stdexcept>

namespace ffca {

    template<typename T>
    class Grid2D {
    public:
        Grid2D(std::size_t width, std::size_t height) : width_(width), height_(height), cells_(width * height) {
            if (width == 0 || height == 0) {
                throw std::invalid_argument("Grid2D dimensions must be nonzero");
            }
        }

        [[nodiscard]] std::size_t width() const noexcept {
            return width_;
        }

        [[nodiscard]] std::size_t height() const noexcept {
            return height_;
        }

        [[nodiscard]] std::size_t size() const noexcept {
            return cells_.size();
        }

        T& operator()(std::size_t row, std::size_t col) {
            return cells_[index(row, col)];
        }

        const T& operator()(std::size_t row, std::size_t col) const {
            return cells_[index(row, col)];
        }

        T& unchecked(std::size_t row, std::size_t col) noexcept{
            return cells_[row * width_ + col];
        }

        const T& unchecked(std::size_t row, std::size_t col) const noexcept {
            return cells_[row * width_ + col];
        }
        T& wrapped(int row, int col) {
            return cells_[wrapped_index(row, col)];
        }

        const T& wrapped(int row, int col) const {
            return cells_[wrapped_index(row, col)];
        }

    private:

        [[nodiscard]] std::size_t index(std::size_t row, std::size_t col) const {
            if (row >= height_ || col >= width_) {
                throw std::out_of_range("Grid2D index out of range");
            }
            return row * width_ + col;
        }

        [[nodiscard]] std::size_t wrap(int value, std::size_t bound) const noexcept {
            int b = static_cast<int>(bound);
            int result = value % b;

            if (result < 0) {
                result += b;
            }
            return static_cast<std::size_t>(result);
        }

        [[nodiscard]] std::size_t wrapped_index(int row, int col) const noexcept {
            std::size_t wrapped_row = wrap(row, height_);
            std::size_t wrapped_col = wrap(col, width_);

            return wrapped_row * width_ + wrapped_col;
        }

        std::size_t width_;
        std::size_t height_;
        std::vector<T> cells_;
    };
} // namespace ffca