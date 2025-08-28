# DiskManagement-Linux library CMakeLists.txt
cmake_minimum_required(VERSION 3.11)
project(disk-management VERSION 1.0.0 LANGUAGES CXX)

# Include FetchContent module
include(FetchContent)

# Create disk-management library
add_library(disk-management STATIC
    src/details.cpp
    src/secure-erase.cpp
    src/gutmann-method.cpp
)

# Set include directories for this library
# BUILD_INTERFACE: Used when building the library
# INSTALL_INTERFACE: Used when the library is installed
target_include_directories(disk-management PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Set C++ standard (requires C++20 for std::format)
target_compile_features(disk-management PUBLIC cxx_std_20)