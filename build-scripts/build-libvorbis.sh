#!/bin/bash

set -euo pipefail
source $(dirname $0)/var.sh

LIB_PATH=third_party/vorbis
CFLAGS="-s $OPTIM_FLAGS -I$BUILD_DIR/include"
LDFLAGS="-L$BUILD_DIR/lib"
CONF_FLAGS=(
  --prefix=$BUILD_DIR
  --host=i686-linux  # use i686 linux
  --enable-shared=no # disable shared library
  --enable-docs=no
  --enable-examples=no
  --enable-fast-install=no
  --disable-oggtest # disable oggtests
)
echo "CONF_FLAGS=${CONF_FLAGS[@]}"

(cd $LIB_PATH &&
  # https://github.com/ffmpegwasm/vorbis/commit/fd1128324e2697b27fb535f009180c520e73664b
  sed -i 's/-mno-ieee-fp//g' configure.ac &&
  emconfigure ./autogen.sh &&
  # https://github.com/kripken/emscripten/issues/264
  sed -i 's/-O20/-O3/g' configure &&
  # disable oggpack_writealign test
  sed -i 's/$ac_cv_func_oggpack_writealign/yes/' configure &&
  CFLAGS=$CFLAGS LDFLAGS=$LDFLAGS emconfigure ./configure "${CONF_FLAGS[@]}")
emmake make -C $LIB_PATH clean
emmake make -C $LIB_PATH install -j
