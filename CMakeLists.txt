
cmake_minimum_required(VERSION 3.16)

project(comp_fys LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(E1
    E1/main.c
)

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

# Maximum optimization for ALL build configurations
target_compile_options(E1 PRIVATE
    -O3
    -march=native
)
