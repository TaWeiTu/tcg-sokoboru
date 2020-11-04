cmake_minimum_required(VERSION 3.15)
project(tcg-sokoboru)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -march=native")

include_directories(${CMAKE_SOURCE_DIR}/abseil-cpp)
add_subdirectory(abseil-cpp)

# libpthread is required
find_package(Threads REQUIRED)

file(GLOB SOURCE "*.cpp" "*.h")
add_executable(solver ${SOURCE})

# Enable link-time optimization
set_property(TARGET solver PROPERTY INTERPROCEDURAL_OPTIMIZATION True)

# Link to pthread and absl::hash
target_link_libraries(solver ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(solver absl::hash)

