# DiskManagement-Linux library CMakeLists.txt
cmake_minimum_required(VERSION 3.11)
project(disk_management VERSION 2.0.0 LANGUAGES CXX)

# Export compile commands for IDE IntelliSense (VSCode, clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

target_include_directories(lshw PRIVATE
    ${LSHW_ROOT}/src/core
)

# =============================================================================
# Disk Management Library
# =============================================================================

# Create the main disk_management library
add_library(disk_management STATIC
    src/ata_disk.cpp
    src/disk.cpp
    src/disk_management.cpp
    src/disk_sanitization_interface.cpp
    src/nist_clear.cpp
    src/nist_purge.cpp
    src/nvme_disk.cpp
    src/secure_erase.cpp
    src/secure_erase_enhanced.cpp
)

# Configure include directories
target_include_directories(disk_management
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/internal
        ${LSHW_ROOT}/src/core
)

# Set C++ standard requirement
target_compile_features(disk_management PUBLIC cxx_std_23)

# Enable strict compiler warnings
target_compile_options(disk_management PRIVATE
    -Wall -Wextra -Wpedantic
    -Wshadow
    -Wnon-virtual-dtor
    -Woverloaded-virtual
    -Wcast-align
    -Wconversion
    -Wsign-conversion
)

# Link external dependencies
target_link_libraries(disk_management
    PRIVATE
        lshw
)