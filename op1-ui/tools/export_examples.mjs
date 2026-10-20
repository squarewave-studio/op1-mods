import { mkdir, writeFile } from 'node:fs/promises';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { scene, text, renderSvg, validateScene } from '../src/components.mjs';
import { playScreen, editorScreen } from '../src/screens.mjs';
import { patterns } from '../src/patterns.mjs';

const root = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const output = resolve(process.argv[2] || resolve(root, 'examples'));
await mkdir(output, { recursive: true });
const typeScale = scene();
text(typeScale, { id: 'small', value: 'SMALL / 11', x: 16, y: 16 });
text(typeScale, { id: 'medium', value: 'MEDIUM / 18', x: 16, y: 44, size: 'medium' });
text(typeScale, { id: 'large', value: '28', x: 16, y: 88, size: 'large' });
const examples = { play: playScreen(), editor: editorScreen(), 'type-scale': typeScale,
  ...Object.fromEntries(patterns.map(p => [p.id, p.build()])) };
for (const [name, scene] of Object.entries(examples)) {
  const errors = validateScene(scene);
  if (errors.length) throw new Error(errors.join('\n'));
  // Metadata is useful in the browser; omit it for the conservative native profile.
  const svg = renderSvg(scene, { title: `${name} reference` }).replace(/<title>.*?<\/title>/, '');
  await writeFile(resolve(output, `${name}.svg`), svg + '\n');
  console.log(`${name}: ${scene.nodes.length} paths, ${Buffer.byteLength(svg)} bytes`);
}

const catalogue = patterns.map(({id,title,lesson}) => ({id,title,lesson}));
await writeFile(resolve(output, 'catalogue.json'), JSON.stringify(catalogue,null,2)+'\n');
const intro = '# Screen Examples\n\nOriginal compositions informed by stock #246 screen families, rendered from code with schematic lettering. These are not stock screens or hardware captures. Every example uses the proposed three text roles and encoder colours. No font loading is needed.\n\nSee [Stock Screen Patterns](STOCK.md) for the inspected resource families, palette identity and evidence limits.\n';
const body = patterns.map(p => `\n## ${p.title}\n\n![${p.title}: original schematic code render](../examples/${p.id}.svg)\n\n${p.lesson}\n`).join('');
await writeFile(resolve(root,'docs/EXAMPLES.md'), intro+body);
