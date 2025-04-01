#!/bin/bash
DIR="$(dirname "$(realpath "$0")")"
BUILD_DIR="${DIR}/build"

mkdir -p "${BUILD_DIR}"
cmake -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release
cmake --build "${BUILD_DIR}"