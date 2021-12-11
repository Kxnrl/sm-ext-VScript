project(VScript)
cmake_minimum_required(VERSION 3.21)

include(configure.cmake)
include(amsdk-cmake/Initialize.cmake)

add_subdirectory(src)

# After project!
include(install.cmake)