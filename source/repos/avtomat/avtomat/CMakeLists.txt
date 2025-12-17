cmake_minimum_required(VERSION 3.14)
project(Calculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- GoogleTest ---
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# --- Library with calculator logic ---
add_library(avtomat
    avtomat.cpp
)

target_include_directories(avtomat
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

# --- Main application ---
add_executable(app
    main.cpp
)

target_link_libraries(app
    PRIVATE avtomat
)

# --- Tests ---
enable_testing()

add_executable(tests
    test.cpp
)

target_link_libraries(tests
    PRIVATE avtomat gtest gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)