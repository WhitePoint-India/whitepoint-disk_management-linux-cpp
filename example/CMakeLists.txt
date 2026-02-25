cmake_minimum_required(VERSION 3.11)
project(Example VERSION 1.0.0 LANGUAGES CXX)

# Include FetchContent module
include(FetchContent)

# Option 1: Fetch disk_management library from GitHub
# FetchContent_Declare(
#     disk_management
#     GIT_REPOSITORY https://github.com/WhitePoint-India/whitepoint-disk_management-linux-cpp.git
#     GIT_TAG main # or specify a specific tag/commit
# )

# Option 2: Fetch disk_management library from local path (useful for development)
FetchContent_Declare(
    disk_management
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Make the library available
FetchContent_MakeAvailable(disk_management)

# Create your executable
add_executable(example src/main.cpp)

# Link the disk_management library
target_link_libraries(example PRIVATE disk_management)

# Set C++ standard
target_compile_features(example PRIVATE cxx_std_23)
