#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

#include "ffca/Engine.hpp"
#include "ffca/Field.hpp"
#include "ffca/Grid2D.hpp"
#include "ffca/RuntimeRules.hpp"
#include "ffca/Rules.hpp"
#include "ffca/Utils.hpp"

template <typename Engine>
double run_single_trial(
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

    return static_cast<double>(elapsed_ns) /
           static_cast<double>(total_cell_updates);
}

template <typename EngineFactory>
void run_benchmark(
    const char* label,
    EngineFactory make_engine,
    std::size_t width,
    std::size_t height,
    std::size_t warmup_steps,
    std::size_t measured_steps,
    std::size_t trials
) {
    std::vector<double> ns_per_cell_results;
    ns_per_cell_results.reserve(trials);

    for (std::size_t trial = 0; trial < trials; ++trial) {
        auto engine = make_engine();

        const double ns_per_cell = run_single_trial(
            engine,
            width,
            height,
            warmup_steps,
            measured_steps
        );

        ns_per_cell_results.emplace_back(ns_per_cell);
    }

    std::ranges::sort(ns_per_cell_results);

    const double best = ns_per_cell_results.front();
    const double median = ns_per_cell_results[trials / 2];
    const double worst = ns_per_cell_results.back();

    std::cout << label
              << " | " << width << "x" << height
              << " | trials: " << trials
              << " | steps/trial: " << measured_steps
              << " | best ns/cell: " << best
              << " | median ns/cell: " << median
              << " | worst ns/cell: " << worst
              << '\n';
}

int main() {
    using F5 = ffca::Fp<5>;

    using RuntimeRule = ffca::RuntimeRule<F5>;
    using RuntimeEngine = ffca::Engine2D<F5, RuntimeRule>;

    using StaticRule = ffca::QuadraticVonNeumannRule<F5>;
    using StaticEngine = ffca::Engine2D<F5, StaticRule>;

    constexpr std::uint32_t seed = 42;
    constexpr std::size_t warmup_steps = 100;
    constexpr std::size_t measured_steps = 1000;
    constexpr std::size_t trials = 5;

    for (std::size_t size : {128, 256, 512, 1024}) {
        run_benchmark(
            "runtime rule",
            [size] {
                ffca::Grid2D<F5> grid{size, size};
                ffca::randomize_grid(grid, seed);

                RuntimeRule rule{
                    .kind = ffca::RuleKind::Quadratic
                };

                return RuntimeEngine{
                    std::move(grid),
                    rule
                };
            },
            size,
            size,
            warmup_steps,
            measured_steps,
            trials
        );

        run_benchmark(
            "static rule ",
            [size] {
                ffca::Grid2D<F5> grid{size, size};
                ffca::randomize_grid(grid, seed);

                StaticRule rule{};

                return StaticEngine{
                    std::move(grid),
                    rule
                };
            },
            size,
            size,
            warmup_steps,
            measured_steps,
            trials
        );

        std::cout << '\n';
    }

    return 0;
}