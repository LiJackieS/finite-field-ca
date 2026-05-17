#pragma once

namespace ffca {

    template <typename Field>
    struct QuadraticVonNeumannRule {
        [[nodiscard]] constexpr Field operator()(
            Field x,
            Field north,
            Field south,
            Field east,
            Field west
        ) const noexcept{
            return x * x + north + south + east + west;
        }
    };

    template <typename Field>
    struct LinearVonNeumannRule {
        Field center_coeff;
        Field neighbor_coeff;

        [[nodiscard]] constexpr Field operator()(
            Field x,
            Field north,
            Field south,
            Field east,
            Field west
        ) const noexcept{
            return center_coeff * x + neighbor_coeff * (north + south + east + west);
        }
    };
} // namespace ffca