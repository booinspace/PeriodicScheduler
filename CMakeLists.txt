cmake_minimum_required(VERSION 3.10)

# Project name
project(PeriodicScheduler)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define source directory variables
set(SRC_DIR "src")
set(INCLUDE_DIR "${SRC_DIR}/include")

# Add source directories
add_library(periodic-scheduler STATIC)
target_include_directories(periodic-scheduler PUBLIC "${INCLUDE_DIR}")
file(GLOB SOURCES "${SRC_DIR}/*.cpp")
target_sources(periodic-scheduler PRIVATE ${SOURCES})

# Add the executable
add_executable(periodic_scheduler "${SRC_DIR}/PeriodicScheduler.cpp")
target_link_libraries(periodic_scheduler PRIVATE periodic-scheduler)

# Setup compiler options
target_compile_options(periodic_scheduler PRIVATE -Wall -Wextra -Wpedantic)
