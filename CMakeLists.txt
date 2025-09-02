# DiskManagement-Linux library CMakeLists.txt
cmake_minimum_required(VERSION 3.11)
project(disk-management VERSION 1.0.0 LANGUAGES CXX)

# Include FetchContent for downloading lshw source
include(FetchContent)

# Download lshw source code
FetchContent_Declare(
    lshw_source
    GIT_REPOSITORY https://github.com/lyonel/lshw.git
    GIT_TAG master
)

# Make lshw source available
FetchContent_Populate(lshw_source)

# Get all source files from lshw core library
file(GLOB LSHW_SOURCES 
    ${lshw_source_SOURCE_DIR}/src/core/*.cc
    ${lshw_source_SOURCE_DIR}/src/core/*.c
)

# Remove files we don't need or that have external dependencies
list(REMOVE_ITEM LSHW_SOURCES 
    ${lshw_source_SOURCE_DIR}/src/core/db.cc  # Requires sqlite3
    ${lshw_source_SOURCE_DIR}/src/core/dump.cc # GUI related
)

# Create lshw library from source files
add_library(lshw STATIC ${LSHW_SOURCES})

# Set compile definitions for lshw
target_compile_definitions(lshw PRIVATE
    VERSION="B.02.19"
    DATADIR="/usr/share"
)

# Set include directories for lshw
target_include_directories(lshw PUBLIC
    ${lshw_source_SOURCE_DIR}/src/core
)

# Create disk-management library
add_library(disk-management STATIC
    src/disk.cpp
    src/secure-erase.cpp
    src/gutmann-method.cpp
    src/disk-management.cpp
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

# Link lshw library
target_link_libraries(disk-management PUBLIC lshw)