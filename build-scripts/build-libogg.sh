#!/bin/bash

set -euo pipefail
source $(dirname $0)/var.sh

LIB_PATH=third_party/Ogg
CFLAGS="-s USE_PTHREADS=1 $OPTIM_FLAGS"
CONF_FLAGS=(
  --prefix=$BUILD_DIR
  --host=i686-linux # use i686 linux
  --disable-shared  # disable shared library
)
echo "CONF_FLAGS=${CONF_FLAGS[@]}"
(cd $LIB_PATH &&
  emconfigure ./autogen.sh &&
  # https://github.com/kripken/emscripten/issues/264
  sed -i 's/-O20/-O3/g' configure &&
  CFLAGS=$CFLAGS emconfigure ./configure "${CONF_FLAGS[@]}")
emmake make -C $LIB_PATH clean
emmake make -C $LIB_PATH install -j
