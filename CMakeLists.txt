cmake_minimum_required(VERSION 3.14)
project(search_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

find_package(Threads REQUIRED)

include_directories(include)

add_executable(search_engine
    src/main.cpp
    src/ConverterJSON.cpp
    src/InvertedIndex.cpp
    src/SearchServer.cpp
)
target_link_libraries(search_engine PRIVATE Threads::Threads)

enable_testing()

add_executable(test_search_engine
    tests/test_search_engine.cpp
    src/InvertedIndex.cpp
    src/SearchServer.cpp
)
target_link_libraries(test_search_engine PRIVATE gtest Threads::Threads)

include(GoogleTest)
gtest_discover_tests(test_search_engine)
