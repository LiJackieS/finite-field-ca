#include <cassert>
#include <iostream>

#include "ffca/Field.hpp"
#include "ffca/Rules.hpp"

int main() {
    using F5 = ffca::Fp<5>;

    {
        ffca::QuadraticVonNeumannRule<F5> rule;

        F5 x{2};
        F5 north{1};
        F5 south{3};
        F5 east{4};
        F5 west{0};

        // x' = x^2 + N + S + E + W
        //    = 2^2 + 1 + 3 + 4 + 0
        //    = 4 + 1 + 3 + 4
        //    = 12
        //    = 2 mod 5
        assert(rule(x, north, south, east, west) == F5{2});
    }

    {
        ffca::LinearVonNeumannRule<F5> rule{
            .center_coeff = F5{2},
            .neighbor_coeff = F5{1}
        };

        F5 x{3};
        F5 north{1};
        F5 south{1};
        F5 east{1};
        F5 west{1};

        // x' = 2x + N + S + E + W
        //    = 2(3) + 1 + 1 + 1 + 1
        //    = 10
        //    = 0 mod 5
        assert(rule(x, north, south, east, west) == F5{0});
    }

    std::cout << "Rule tests passed.\n";
}