import { scene, line, text, parameter, header, softKeys, envelope } from './components.mjs';

// Original playing and editor layouts. Controls are illustrative.
export function playScreen({ values = [40, 40, 40, 40], shift = false } = {}) {
  const s = scene();
  const labels = shift ? ['MORPH', 'MOD', 'NOISE', 'CUTOFF'] : ['MORPH', 'PITCH', 'DECAY', 'BALANCE'];
  [0, 1, 2, 3].forEach(i => parameter(s, { id: `p${i}`, label: labels[i], value: String(values[i]), lane: i, x: i < 2 ? 9 : 311, align: i < 2 ? 'left' : 'right', y: i % 2 ? 75 : 9, size: 'large', width: 62 }));
  // A simple sound bridge: a readable musical object with controls at its edges.
  line(s, 'bridge-base', [[92, 114], [228, 114]], 'secondary');
  line(s, 'bridge-left', [[105, 114], [105, 47], [115, 47], [115, 114]], 'secondary');
  line(s, 'bridge-right', [[205, 114], [205, 47], [215, 47], [215, 114]], 'secondary');
  const amount = values[0] / 100 * 12;
  const points = Array.from({ length: 17 }, (_, i) => [108 + i * 6.5, 78 + Math.sin(i * Math.PI / 4) * amount]);
  line(s, 'bridge-sound', points, 'blue', 1.5);
  text(s, { id: 'bridge-name', value: 'SOUND', x: 160, y: 30, align: 'center' });
  softKeys(s, ['EDIT', 'ENV', 'FX', 'LFO']);
  return s;
}

export function editorScreen({ attack = 145, decay = 158, side = 'A' } = {}) {
  const s = scene();
  header(s, { title: 'DRY SNARE', context: `EDIT ${side}`, right: 'TONE ENV' });
  envelope(s, { attack: Math.min(1, attack / 400), decay: Math.min(1, decay / 400) });
  parameter(s, { id: 'morph', label: 'MORPH', value: '40', lane: 0, x: 9, y: 88 });
  parameter(s, { id: 'attack', label: 'ATTACK', value: String(attack), unit: 'MS', lane: 1, x: 89, y: 88 });
  parameter(s, { id: 'decay', label: 'DECAY', value: String(decay), unit: 'MS', lane: 2, x: 169, y: 88 });
  softKeys(s, ['EXIT', 'PREV', 'NEXT', 'NAME']);
  return s;
}
