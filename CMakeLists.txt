
cmake_minimum_required(VERSION 3.16)

project(comp_fys LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(E1
    E1/main.c
)

# Maximum optimization for ALL build configurations
target_compile_options(E1 PRIVATE
    -O3
    -march=native
)
