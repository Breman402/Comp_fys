
cmake_minimum_required(VERSION 3.16)

project(comp_fys LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

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
find_package(GSL QUIET)

if(NOT GSL_FOUND)
    message(STATUS "GSL not installed. Fetching GSL...")

    include(FetchContent)

    # Disable unnecessary GSL features
    set(NO_AMPL_BINDINGS ON CACHE BOOL "" FORCE)
    set(GSL_DISABLE_TESTS ON CACHE BOOL "" FORCE)
    set(GSL_INSTALL OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        gsl
        GIT_REPOSITORY https://github.com/ampl/gsl.git
        GIT_TAG master
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(gsl)

    # Provide the same target as find_package(GSL)
    add_library(GSL::gsl ALIAS gsl)

    message(STATUS "GSL fetched successfully!")
else()
    message(STATUS "Using installed GSL: ${GSL_VERSION}")
endif()

add_executable(gsl_example
    C-introduction/C2/gsl_example.c
)

add_executable(C2
    C-introduction/C2/main.c
    C-introduction/C2/ownPairDistance.c
    C-introduction/C2/ownScalar.c
)

target_link_libraries(gsl_example PRIVATE GSL::gsl)
target_link_libraries(C2 PRIVATE GSL::gsl m)


# C3 Code: ----------------------------------------------------
add_executable(fix_me
    C-introduction/C3/debugging/fix_me.c
)

target_link_libraries(fix_me PRIVATE m)

# E1 Code: ----------------------------------------------------
add_executable(E1
    E1/main.c
)

add_executable(task_6
    E1/task_6.c
)

add_executable(task_7
    E1/task_7.c
)

target_link_libraries(task_7 PRIVATE m)

# Maximum optimization for E1
target_compile_options(E1 PRIVATE
    -O3
    -march=native
)
