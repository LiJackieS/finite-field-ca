#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <utility>

#include "ffca/Engine.hpp"
#include "ffca/Field.hpp"
#include "ffca/Grid2D.hpp"
#include "ffca/RuntimeRules.hpp"
#include "ffca/Utils.hpp"

template <typename Engine>
void run_benchmark(
    Engine& engine,
    std::size_t width,
    std::size_t height,
    std::size_t warmup_steps,
    std::size_t measured_steps
) {
    for (std::size_t i = 0; i < warmup_steps; ++i) {
        engine.step();
    }

    const auto start = std::chrono::steady_clock::now();

    for (std::size_t i = 0; i < measured_steps; ++i) {
        engine.step();
    }

    const auto end = std::chrono::steady_clock::now();

    const auto elapsed_ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    const std::size_t total_cell_updates = width * height * measured_steps;

    const double ns_per_cell =
        static_cast<double>(elapsed_ns) / static_cast<double>(total_cell_updates);

    const double elapsed_seconds =
        static_cast<double>(elapsed_ns) / 1'000'000'000.0;

    const double steps_per_second =
        static_cast<double>(measured_steps) / elapsed_seconds;

    std::cout << width << "x" << height
              << " | steps: " << measured_steps
              << " | total time: " << elapsed_ns / 1'000'000.0 << " ms"
              << " | ns/cell: " << ns_per_cell
              << " | steps/sec: " << steps_per_second
              << '\n';
}

int main() {
    using F5 = ffca::Fp<5>;
    using Rule = ffca::RuntimeRule<F5>;
    using Engine = ffca::Engine2D<F5, Rule>;

    constexpr std::uint32_t seed = 42;
    constexpr std::size_t warmup_steps = 100;
    constexpr std::size_t measured_steps = 1000;

    for (std::size_t size : {128, 256, 512, 1024}) {
        ffca::Grid2D<F5> grid{size, size};
        ffca::randomize_grid(grid, seed);

        Rule rule{.kind = ffca::RuleKind::Quadratic};

        Engine engine{std::move(grid), rule};

        run_benchmark(engine, size, size, warmup_steps, measured_steps);
    }

    return 0;
}