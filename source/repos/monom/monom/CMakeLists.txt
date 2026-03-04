cmake_minimum_required(VERSION 3.14)
project(Calculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Header-only library ---
add_library(monom INTERFACE)
target_include_directories(monom
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

# --- Main application ---
add_executable(app main.cpp)
target_link_libraries(app PRIVATE monom)

# --- GoogleTest integration ---
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

if(WIN32)
    # Чтобы GTest использовал правильный CRT на Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(googletest)

enable_testing()

# --- Tests ---
add_executable(tests test.cpp)
target_link_libraries(tests PRIVATE monom GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)