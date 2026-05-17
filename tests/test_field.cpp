#include <cassert>
#include <iostream>

#include "ffca/Field.hpp"

int main() {
    using ffca::Fp;
    using F5 = Fp<5>;

    static_assert(F5{7}.value() == 2);
    static_assert((F5{3} + F5{4}).value() == 2);
    static_assert((F5{4} * F5{4}).value() == 1);
    static_assert((F5{1} - F5{3}).value() == 3);
    static_assert((-F5{2}).value() == 3);

    F5 a{3};
    F5 b{4};

    assert((a + b).value() == 2);
    assert((a - b).value() == 4);
    assert((b - a).value() == 1);
    assert((a * b).value() == 2);
    assert((-b).value() == 1);
    assert(F5::modulus() == 5);

    std::cout << "Field tests passed.\n";
}