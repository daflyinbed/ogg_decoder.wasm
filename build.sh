#!/bin/bash

set -eo pipefail

emcc -v

SCRIPT_ROOT=$(dirname $0)/build-scripts

# install dependencies
$SCRIPT_ROOT/install-deps.sh

#build libogg
$SCRIPT_ROOT/build-libogg.sh

#build libvorbis
$SCRIPT_ROOT/build-libvorbis.sh

emcc -O3 -s LLD_REPORT_UNDEFINED \
    -s EXPORTED_FUNCTIONS="['_decode']" \
    -s EXPORTED_RUNTIME_METHODS='["ccall"]' \
    -Ibuild/include -Lbuild/lib -lvorbis -lvorbisfile -logg src/main.c -o build/libvorbis.js