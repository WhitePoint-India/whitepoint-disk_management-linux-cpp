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
# libnvme External Dependency (Git Submodule, built via Meson)
# =============================================================================
#
# libnvme uses Meson (not CMake) and generates config/version headers at
# configure time, so it cannot be globbed into a target like lshw. Instead we
# build it as a static archive via ExternalProject_Add and import the result.
# All optional deps (OpenSSL, json-c, dbus, keyutils, liburing) are disabled so
# the archive is self-contained for in-band sanitize/format use.

include(ExternalProject)

set(LIBNVME_SRC     ${CMAKE_CURRENT_SOURCE_DIR}/external/libnvme)
set(LIBNVME_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/libnvme-install)
set(LIBNVME_STATIC  ${LIBNVME_INSTALL}/lib/libnvme.a)
set(LIBNVME_INCLUDE ${LIBNVME_INSTALL}/include)

if(NOT EXISTS ${LIBNVME_SRC}/meson.build)
    message(FATAL_ERROR
        "libnvme submodule not found. Please run:\n"
        "  git submodule update --init --recursive")
endif()

ExternalProject_Add(libnvme_ext
    SOURCE_DIR ${LIBNVME_SRC}
    PREFIX     ${CMAKE_CURRENT_BINARY_DIR}/libnvme-build
    CONFIGURE_COMMAND meson setup <BINARY_DIR> <SOURCE_DIR>
        --prefix=${LIBNVME_INSTALL}
        --libdir=lib
        --default-library=static
        --buildtype=release
        -Dpython=disabled
        -Dopenssl=disabled
        -Djson-c=disabled
        -Dlibdbus=disabled
        -Dkeyutils=disabled
        -Dliburing=disabled
        -Dexamples=false
        -Dtests=false
        -Ddocs=false
    BUILD_COMMAND   meson compile -C <BINARY_DIR>
    INSTALL_COMMAND meson install -C <BINARY_DIR>
    BUILD_BYPRODUCTS ${LIBNVME_STATIC}
)

# Wrap the built archive as an imported target. The include dir must exist at
# configure time, before ExternalProject has run.
file(MAKE_DIRECTORY ${LIBNVME_INCLUDE})
add_library(libnvme STATIC IMPORTED GLOBAL)
set_target_properties(libnvme PROPERTIES
    IMPORTED_LOCATION             ${LIBNVME_STATIC}
    INTERFACE_INCLUDE_DIRECTORIES ${LIBNVME_INCLUDE})
add_dependencies(libnvme libnvme_ext)

# =============================================================================
# Disk Management Library
# =============================================================================

# Create the main disk_management library
add_library(disk_management STATIC
    src/ata_disk.cpp
    src/disk.cpp
    src/disk_management.cpp
    src/disk_sanitization_interface.cpp
    src/lshw_hardware_detector.cpp
    src/nist_clear.cpp
    src/nist_purge.cpp
    src/nvme_disk.cpp
    src/sanitization_method_registry.cpp
    src/secure_erase.cpp
    src/sanitization_progress.cpp
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
        libnvme
)

# Ensure libnvme is built and its headers installed before compiling our
# sources (nvme_disk.cpp includes <libnvme.h>). Linking alone only orders the
# link step, not compilation.
add_dependencies(disk_management libnvme_ext)