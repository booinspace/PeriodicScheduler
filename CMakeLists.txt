cmake_minimum_required(VERSION 3.10)

# Project name
project(PeriodicScheduler)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define source directory variables
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${SRC_DIR}/include")
file(GLOB SOURCES "${SRC_DIR}/*.cpp")
add_executable(periodic_scheduler ${SOURCES})
target_include_directories(periodic_scheduler PUBLIC ${INCLUDE_DIR})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Building with Release")
else()
    message(STATUS "Building with Debug")
    add_compile_definitions(DEBUG)
endif()

# Setup compiler options
target_compile_options(periodic_scheduler PRIVATE -Wall -Wextra -Wpedantic)

# Google Test setup
if(BUILD_TESTS STREQUAL "ON")
    # Fetch and configure Google Test
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    FetchContent_MakeAvailable(googletest)

    add_subdirectory("${SRC_DIR}/test")

    # Enable testing
    enable_testing()
endif()