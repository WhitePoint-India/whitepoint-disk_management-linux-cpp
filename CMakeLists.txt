# DiskManagement-Linux library CMakeLists.txt
cmake_minimum_required(VERSION 3.11)
project(disk-management VERSION 1.0.0 LANGUAGES CXX)

# =============================================================================
# lshw External Dependency (Git Submodule)
# =============================================================================

# Check if lshw submodule is properly initialized
set(LSHW_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/external/lshw)
if(NOT EXISTS ${LSHW_ROOT}/src/core)
    message(FATAL_ERROR 
        "lshw submodule not found. Please run:\n"
        "  git submodule update --init --recursive")
endif()

# Collect lshw source files
file(GLOB LSHW_SOURCES 
    ${LSHW_ROOT}/src/core/*.cc
    ${LSHW_ROOT}/src/core/*.c
)

# Remove files with external dependencies that we don't need
list(REMOVE_ITEM LSHW_SOURCES 
    ${LSHW_ROOT}/src/core/db.cc      # Requires sqlite3
    ${LSHW_ROOT}/src/core/dump.cc    # GUI-related functionality
)

# Build lshw as a static library
add_library(lshw STATIC ${LSHW_SOURCES})

target_compile_definitions(lshw PRIVATE
    VERSION="B.02.19"
    DATADIR="/usr/share"
)

target_include_directories(lshw PUBLIC
    ${LSHW_ROOT}/src/core
)

# =============================================================================
# Disk Management Library
# =============================================================================

# Create the main disk-management library
add_library(disk-management STATIC
    src/disk.cpp
    src/secure-erase.cpp
    src/gutmann-method.cpp
    src/disk-management.cpp
)

# Configure include directories
target_include_directories(disk-management PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Set C++ standard requirement
target_compile_features(disk-management PUBLIC cxx_std_20)

# Link external dependencies
target_link_libraries(disk-management PUBLIC lshw)