# CMake version.
cmake_minimum_required(VERSION 3.10)

# Project.
project(leaf-disk-gen VERSION 0.1 LANGUAGES CXX)

# Set release.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set C compile flags.
set(CMAKE_C_FLAGS "-Wall -Wextra -march=native -mtune=native")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

# Set C++ compile flags.
set(CMAKE_CXX_FLAGS "-Wall -Wextra -march=native -mtune=native")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Set C++17.
function(set_target_cxx17 TARGET_ARG)
    set_target_properties(
        ${TARGET_ARG}
        PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED True
        )
endfunction(set_target_cxx17)

# Set common include directories.
function(set_target_common_include_directories TARGET_ARG)
    target_include_directories(
        ${TARGET_ARG}
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/preform/include"
        )
endfunction(set_target_common_include_directories)

# Add executable.
add_executable(
    leaf-disk-gen
    "${CMAKE_CURRENT_SOURCE_DIR}/src/leaf_angle_distribution.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/leaf_disk.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    )
set_target_cxx17(leaf-disk-gen)
set_target_common_include_directories(leaf-disk-gen)
