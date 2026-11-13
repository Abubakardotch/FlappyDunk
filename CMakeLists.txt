cmake_minimum_required(VERSION 3.28)

project(FlappyDunk LANGUAGES CXX)

include(FetchContent)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SFML
    URL https://github.com/SFML/SFML/archive/refs/tags/3.1.0.zip
)

FetchContent_MakeAvailable(SFML)

add_executable(
    FlappyDunk
    "FlappyDunk.cpp"
)

target_compile_features(
    FlappyDunk
    PRIVATE
    cxx_std_17
)

target_link_libraries(
    FlappyDunk
    PRIVATE
    SFML::Graphics
)