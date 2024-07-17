cmake_minimum_required (VERSION 3.6)
project(example)

add_executable(DiscordDialog src/source.cpp)

add_subdirectory(deps/sleepy-discord)
target_link_libraries(DiscordDialog sleepy-discord)