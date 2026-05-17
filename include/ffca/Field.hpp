#pragma once

#include <cstdint>
#include <ostream>

namespace ffca { // Helps out with prevent naming collisions

    constexpr bool is_prime(std::uint32_t num) {
        if (num < 2) {
            return false;
        }
        for (std::uint64_t d = 2; d * d <= num; ++d) { // We avoid having to check more integers than needed to prove that it is prime
            if (num % d == 0) {
                return false;
            }
        }
        return true;
    }

    template<std::uint32_t P>
    class Fp {
        static_assert(is_prime(P), "Fp<P> requires P to be prime");

    public:
        constexpr Fp() noexcept : value_(0) {}

        constexpr explicit Fp(std::uint64_t value) noexcept : value_(static_cast<std::uint32_t>(value % P)) {}

        [[nodiscard]] constexpr std::uint32_t value() const noexcept {
            return value_;
        }

        [[nodiscard]] static constexpr std::uint32_t modulus() noexcept {
            return P;
        }

        [[nodiscard]] constexpr Fp operator+(Fp rhs) const noexcept { // Addition overload
            return Fp{static_cast<std::uint64_t>(value_) + rhs.value_};
        }

        [[nodiscard]] constexpr Fp operator-(Fp rhs) const noexcept { // subtraction overload
            return Fp{static_cast<std::uint64_t>(value_) + P - rhs.value_};
        }

        [[nodiscard]] constexpr Fp operator*(Fp rhs) const noexcept { // multiplication overload
            return Fp{static_cast<std::uint64_t>(value_) * rhs.value_};
        }

        [[nodiscard]] constexpr Fp operator-() const noexcept { // negation overload
            return value_ == 0 ? Fp{0} : Fp{P - value_};
        }

        constexpr Fp& operator+=(Fp rhs) noexcept {
            *this = *this + rhs;
            return *this;
        }

        constexpr Fp& operator-=(Fp rhs) noexcept {
            *this = *this - rhs;
            return *this;
        }

        constexpr Fp& operator*=(Fp rhs) noexcept {
            *this = *this * rhs;
            return *this;
        }

        [[nodiscard]] constexpr bool operator==(Fp rhs) const noexcept {
            return value_ == rhs.value_;
        }

        [[nodiscard]] constexpr bool operator!=(Fp rhs) const noexcept {
            return !(*this == rhs);
        }

    private:
        std::uint32_t value_;
    };

    template <std::uint32_t P>
    std::ostream& operator<<(std::ostream& os, Fp<P> x) {
        return os << x.value();
    }
} // namespace ffca