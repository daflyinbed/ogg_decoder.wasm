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
emcc -O3 --closure 1 \
    -s LLD_REPORT_UNDEFINED \
    -s MODULARIZE=1 \
    -s EXPORTED_FUNCTIONS="['_decode']" \
    -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "FS"]' \
    -s EXPORT_NAME="createOggDecoder" \
    -Ibuild/include -Lbuild/lib -lvorbis -lvorbisfile -logg \
    src/main.c \
    -o build/libvorbis.mjs

cp src/index.js build/
cp src/index.d.ts build/