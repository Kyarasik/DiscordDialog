cmake_minimum_required (VERSION 3.6)
project(example)

add_executable(minion src/source.cpp)

add_subdirectory(deps/sleepy-discord)
target_link_libraries(minion sleepy-discord)