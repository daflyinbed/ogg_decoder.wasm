import { EmscriptenModule } from "@types/emscripten";
type Instance = EmscriptenModule;
export function decode(
  buf: Uint8Array,
  instance: Instance
): [false, null] | [true, Uint8Array];
export function toBlobUrl(buf: Uint8Array): string;
export async function createInstance(module: EmscriptenModule): Instance;
