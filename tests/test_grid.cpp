#include <cassert>
#include <iostream>

#include "ffca/Field.hpp"
#include "ffca/Grid2D.hpp"

int main() {
    using F5 = ffca::Fp<5>;

    ffca::Grid2D<F5> grid{3, 2};

    assert(grid.width() == 3);
    assert(grid.height() == 2);
    assert(grid.size() == 6);

    grid(0, 0) = F5{1};
    grid(0, 1) = F5{2};
    grid(0, 2) = F5{3};
    grid(1, 0) = F5{4};
    grid(1, 1) = F5{5};
    grid(1, 2) = F5{6};

    assert(grid(0, 0) == F5{1});
    assert(grid(0, 1) == F5{2});
    assert(grid(0, 2) == F5{3});
    assert(grid(1, 0) == F5{4});
    assert(grid(1, 1) == F5{0});
    assert(grid(1, 2) == F5{1});

    assert(grid.wrapped(-1, 0) == grid(1, 0));
    assert(grid.wrapped(2, 0) == grid(0, 0));
    assert(grid.wrapped(0, -1) == grid(0, 2));
    assert(grid.wrapped(0, 3) == grid(0, 0));
    assert(grid.wrapped(-1, -1) == grid(1, 2));

    std::cout << "Grid tests passed.\n";
}