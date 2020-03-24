cmake_minimum_required (VERSION 3.13.0)
project (alt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#SET(GCC_COMPILE_FLAGS "-mavx2")
SET(GCC_COMPILE_FLAGS "-msse4.1 -D_GNU_SOURCE")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}")

include_directories(${CMAKE_SOURCE_DIR}/src)

add_subdirectory(src)
