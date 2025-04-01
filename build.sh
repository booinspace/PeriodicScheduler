#!/bin/bash
DIR="$(dirname "$(realpath "$0")")"
BUILD_DIR="${DIR}/build"

TYPE=$1

mkdir -p "${BUILD_DIR}"
cmake -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=${TYPE}
cmake --build "${BUILD_DIR}"