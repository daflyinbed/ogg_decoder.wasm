import createOggDecoder from "./libvorbis.mjs";
export async function createInstance(module) {
  return createOggDecoder(module);
}
export function decode(buf, instance) {
  instance.FS.writeFile("/decode.ogg", buf);
  let result = instance.ccall(
    "decode",
    "number",
    ["string", "string"],
    ["/decode.ogg", "/decode.wav"]
  );
  if (result === 0) {
    let buf = instance.FS.readFile("/decode.wav");
    instance.FS.unlink("/decode.ogg");
    instance.FS.unlink("/decode.wav");
    return [true, buf];
  }
  instance.FS.unlink("/decode.ogg");
  return [false, null];
}
export function toBlobUrl(buf) {
  let blob = new Blob([buf], {
    type: "application/octet-stream",
  });
  return window.URL.createObjectURL(blob);
}
