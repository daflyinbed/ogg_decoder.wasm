# ogg_decoder.wasm
A simple .ogg(vorbis) decoder in wasm, ported using Emscripten.
# Usage
Put libvorbis.wasm in your bundler's output dictionary. 

* The Emscripten load .wasm through xhr whose path calculated in runtime, so most bundler CAN NOT handle it correctly. [The issue in emscripten](https://github.com/emscripten-core/emscripten/issues/13571)

* If you are using webpack4 try [wasm-module-webpack-plugin](https://github.com/zhouzhi3859/wasm-module-webpack-plugin)
```js
import { createInstance, decode, toBlobUrl } from "index.js";
let instance = await createInstance();
let [success, result] = decode(new Uint8Array(audio_buf), instance)
// result will be an Uint8Array (.wav) if success
if (success) {
  let url = toBlobUrl(result);
  document.getElementById("player").setAttribute("src", url);
}
```
you can send an Module object to createInstance to control the execution of code. [Learn more](https://emscripten.org/docs/api_reference/module.html)
```js
let instance = await createInstance({printErr: (text) => {console.log(text)}});
```
# Build_from_source
```sh
$ git clone https://github.com/daflyinbed/ogg_decoder.wasm.git
$ git submodule update --init --recursive
$ ./build-with-docker.sh
```
If nothing goes wrong, you can find build/libvorbis.wasm build/libvorbis.mjs index.d.ts
# Relative repos
[libogg](https://gitlab.xiph.org/xiph/ogg)

[libvorbis](https://gitlab.xiph.org/xiph/vorbis)

[vorbis tools](https://gitlab.xiph.org/xiph/vorbis-tools)

[ffmepg.wasm](https://github.com/ffmpegwasm/ffmpeg.wasm)

[ffmpeg.wasm-core](https://github.com/ffmpegwasm/ffmpeg.wasm-core)

[vorbis.js](https://github.com/audiocogs/vorbis.js)
