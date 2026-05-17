#pragma once

namespace ffca {

enum class RuleKind {
    Quadratic,
    Linear,
    NeighborSum,
    Multiplicative
};

inline const char* rule_name(RuleKind kind) {
    switch (kind) {
        case RuleKind::Quadratic:
            return "Quadratic: x^2 + N + S + E + W";
        case RuleKind::Linear:
            return "Linear: x + N + S + E + W";
        case RuleKind::NeighborSum:
            return "Neighbor sum: N + S + E + W";
        case RuleKind::Multiplicative:
            return "Multiplicative: xN + S + E + W";
    }

    return "Unknown";
}

template <typename Field>
struct RuntimeRule {
    RuleKind kind = RuleKind::Quadratic;

    [[nodiscard]] constexpr Field operator()(
        Field x,
        Field north,
        Field south,
        Field east,
        Field west
    ) const noexcept {
        switch (kind) {
            case RuleKind::Quadratic:
                return x * x + north + south + east + west;

            case RuleKind::Linear:
                return x + north + south + east + west;

            case RuleKind::NeighborSum:
                return north + south + east + west;

            case RuleKind::Multiplicative:
                return x * north + south + east + west;
        }

        return x;
    }
};

} // namespace ffca