import { mkdir, writeFile } from 'node:fs/promises';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { scene, text, renderSvg, validateScene } from '../src/components.mjs';
import { playScreen, editorScreen, libraryScreen } from '../src/screens.mjs';

const root = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const output = resolve(process.argv[2] || resolve(root, 'examples'));
await mkdir(output, { recursive: true });
const typeScale = scene();
text(typeScale, { id: 'small', value: 'SMALL / 11', x: 16, y: 16 });
text(typeScale, { id: 'medium', value: 'MEDIUM / 18', x: 16, y: 44, size: 'medium' });
text(typeScale, { id: 'large', value: '28', x: 16, y: 88, size: 'large' });
const examples = { play: playScreen(), editor: editorScreen(), library: libraryScreen(), 'type-scale': typeScale,
  'library-empty': libraryScreen({ items: [] }), 'library-error': libraryScreen({ state: 'error' }),
  'library-loading': libraryScreen({ state: 'loading' }) };
for (const [name, scene] of Object.entries(examples)) {
  const errors = validateScene(scene);
  if (errors.length) throw new Error(errors.join('\n'));
  // Metadata is useful in the browser; omit it for the conservative native profile.
  const svg = renderSvg(scene, { title: `${name} reference` }).replace(/<title>.*?<\/title>/, '');
  await writeFile(resolve(output, `${name}.svg`), svg + '\n');
  console.log(`${name}: ${scene.nodes.length} paths, ${Buffer.byteLength(svg)} bytes`);
}
