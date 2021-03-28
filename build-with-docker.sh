EM_VERSION=2.0.12

docker pull emscripten/emsdk:$EM_VERSION

docker run \
    --rm \
    -v $PWD:/src \
    -v $PWD/build:/src/build \
    emscripten/emsdk:$EM_VERSION \
    sh -c "bash ./build.sh"
