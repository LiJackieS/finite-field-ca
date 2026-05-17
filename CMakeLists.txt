cmake_minimum_required(VERSION 3.20)

project(finite_field_ca LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(test_field
    tests/test_field.cpp
)

target_include_directories(test_field
    PRIVATE
        include
)
add_executable(test_grid
    tests/test_grid.cpp
)

target_include_directories(test_grid
    PRIVATE
        include
)