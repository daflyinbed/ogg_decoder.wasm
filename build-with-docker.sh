EM_VERSION=3.1.15

docker pull emscripten/emsdk:$EM_VERSION

docker run \
    --rm \
    -v $PWD:/src \
    -v $PWD/build:/src/build \
    emscripten/emsdk:$EM_VERSION \
    sh -c "bash ./build.sh"
