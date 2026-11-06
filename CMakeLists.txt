
cmake_minimum_required(VERSION 3.16)

project(comp_fys LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(E1
    E1/main.c
)

# C1 Code: ----------------------------------------------------
add_executable(types
    C-introduction/C1/types.c
)

add_executable(pointers
    C-introduction/C1/pointers.c
)

add_executable(stack
    C-introduction/C1/stack.c
)

add_executable(heap
    C-introduction/C1/heap.c
)

add_executable(scopes
    C-introduction/C1/scopes.c
)

add_executable(functions
    C-introduction/C1/functions.c
)

add_executable(stack_array
    C-introduction/C1/stack_array.c
)

add_executable(uninitialized
    C-introduction/C1/uninitialized.c
)
# C2 Code: ----------------------------------------------------
find_package(GSL REQUIRED)

add_executable(gsl_example
    C-introduction/C2/gsl_example.c
)

target_link_libraries(gsl_example PRIVATE GSL::gsl)

# C3 Code: ----------------------------------------------------
add_executable(fix_me
    C-introduction/C3/debugging/fix_me.c
)

target_link_libraries(fix_me PRIVATE m)

# Maximum optimization for E1
target_compile_options(E1 PRIVATE
    -O3
    -march=native
)
