cmake_minimum_required(VERSION 3.10)

# Project name
project(PeriodicScheduler)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define source directory variables
set(SRC_DIR "src")
set(INCLUDE_DIR "${SRC_DIR}/include")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")
add_executable(periodic_scheduler ${SOURCES})
target_include_directories(periodic_scheduler PUBLIC ${INCLUDE_DIR})

# Setup compiler options
target_compile_options(periodic_scheduler PRIVATE -Wall -Wextra -Wpedantic)
