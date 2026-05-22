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
#include "ffca/Rules.hpp"
#include "ffca/Utils.hpp"

template <typename Engine, typename StepFn>
double run_single_trial(
    Engine& engine,
    StepFn step_fn,
    std::size_t width,
    std::size_t height,
    std::size_t warmup_steps,
    std::size_t measured_steps
) {
    for (std::size_t i = 0; i < warmup_steps; ++i) {
        step_fn(engine);
    }

    const auto start = std::chrono::steady_clock::now();

    for (std::size_t i = 0; i < measured_steps; ++i) {
        step_fn(engine);
    }

    const auto end = std::chrono::steady_clock::now();

    const auto elapsed_ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    const std::size_t total_cell_updates = width * height * measured_steps;

    return static_cast<double>(elapsed_ns) /
           static_cast<double>(total_cell_updates);
}

template <typename EngineFactory, typename StepFn>
void run_benchmark(
    const char* label,
    EngineFactory make_engine,
    StepFn step_fn,
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
            step_fn,
            width,
            height,
            warmup_steps,
            measured_steps
        );

        ns_per_cell_results.emplace_back(ns_per_cell);
    }

    std::ranges::sort(ns_per_cell_results);

    const double best = ns_per_cell_results.front();
    const double median = ns_per_cell_results[ns_per_cell_results.size() / 2];
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
    using Rule = ffca::QuadraticVonNeumannRule<F5>;
    using Engine = ffca::Engine2D<F5, Rule>;

    constexpr std::uint32_t seed = 42;
    constexpr std::size_t warmup_steps = 20;
    constexpr std::size_t measured_steps = 100;
    constexpr std::size_t trials = 5;

    for (std::size_t size : {256, 512, 1024}) {
        auto make_engine = [size] {
            ffca::Grid2D<F5> grid{size, size};
            ffca::randomize_grid(grid, seed);

            Rule rule{};

            return Engine{
                std::move(grid),
                rule
            };
        };

        run_benchmark(
            "scalar      ",
            make_engine,
            [](Engine& engine) {
                engine.step();
            },
            size,
            size,
            warmup_steps,
            measured_steps,
            trials
        );

        for (std::size_t threads : {2, 4, 8}) {
            run_benchmark(
                threads == 2 ? "parallel 2  "
              : threads == 4 ? "parallel 4  "
                              : "parallel 8  ",
                make_engine,
                [threads](Engine& engine) {
                    engine.step_parallel(threads);
                },
                size,
                size,
                warmup_steps,
                measured_steps,
                trials
            );
        }

        std::cout << '\n';
    }

    return 0;
}