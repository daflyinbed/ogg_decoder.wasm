#!/bin/bash
#
# Common variables for all scripts

set -euo pipefail

OPTIM_FLAGS=(
    -O3
)

OPTIM_FLAGS=(
    "${OPTIM_FLAGS[@]}"
    --closure 1
)

# Convert array to string
OPTIM_FLAGS="${OPTIM_FLAGS[@]}"

# Root directory
ROOT_DIR=$PWD

# Directory to install headers and libraries
BUILD_DIR=$ROOT_DIR/build

echo "OPTIM_FLAGS=$OPTIM_FLAGS"
echo "BUILD_DIR=$BUILD_DIR"
