cmake_minimum_required(VERSION 3.16)
project(swos-port LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(SWOS_EMBED_ASSETS "Embed assets in executable" OFF)

# Only the 32-bit generated VM code exists; it works on 64-bit via offset-based memory
set(PLATFORM_ARCH "32")

# Use swos-port directory as the project root for paths
# The actual source files reference paths relative to the swos-port parent
set(SWOS_ROOT "${CMAKE_SOURCE_DIR}")

add_subdirectory(src)
