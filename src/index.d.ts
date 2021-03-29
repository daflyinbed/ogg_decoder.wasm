import { EmscriptenModule } from "@types/emscripten";
type Instance = EmscriptenModule;
export function decode(
  buf: Uint8Array,
  instance: Instance
): [boolean, Uint8Array];
export function toBlobUrl(buf: Uint8Array): string;
export async function createInstance(module: EmscriptenModule): Instance;
