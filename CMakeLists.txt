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
add_executable(test_rules
    tests/test_rules.cpp
)

target_include_directories(test_rules
    PRIVATE
        include
)

add_executable(test_engine
    tests/test_engine.cpp
)

target_include_directories(test_engine
    PRIVATE
        include
)

add_executable(ffca_demo
    src/main.cpp
)

target_include_directories(ffca_demo
    PRIVATE
        include
)

find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2)

add_executable(ffca_visualizer
    src/visualize.cpp
)

target_include_directories(ffca_visualizer
    PRIVATE
        include
)

target_link_libraries(ffca_visualizer
    PRIVATE
        SDL2::SDL2
)