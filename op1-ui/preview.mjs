import { scene, text, renderSvg, validateScene, tokens } from './src/components.mjs';
import { playScreen, editorScreen, libraryScreen } from './src/screens.mjs';
import { loadStockFont, applyStockFont } from './src/stock-font.mjs';

const $ = id => document.getElementById(id);
let view = 'play', values = [40, 40, 40, 40], selected = 1, font = null;
const keyLabels = { play: ['EDIT', 'ENV', 'FX', 'LFO'], editor: ['EXIT', 'PREV', 'NEXT', 'NAME'], library: ['BACK', 'INFO', 'LOAD', 'TOOLS'] };
function draw() {
  const state = $('browser-state').value;
  let s = view === 'play' ? playScreen({ values, shift: $('shift').checked }) : view === 'editor' ? editorScreen({ attack: values[1] * 4, decay: values[2] * 4 }) : libraryScreen({ selected, state, ...(state === 'empty' ? { items: [] } : {}) });
  $('screen').innerHTML = renderSvg(s, { guides: $('guides').checked });
  const issues = validateScene(s);
  if (font) { try { issues.push(...applyStockFont($('screen').querySelector('svg'), s, font)); } catch (error) { issues.push(error.message); } }
  $('validation').textContent = issues.join(' / ');
  $('screen').style.width = `${320 * Number($('zoom').value)}px`;
  $('screen').style.height = `${160 * Number($('zoom').value)}px`;
  const keys = view === 'library' && state === 'error' ? ['BACK', '', 'RETRY', ''] : view === 'library' && state !== 'ready' ? ['BACK', '', '', ''] : keyLabels[view];
  keys.forEach((label, i) => { $(`key${i + 1}`).textContent = `T${i + 1} · ${label || '—'}`; });
  document.querySelectorAll('[data-view]').forEach(b => b.setAttribute('aria-pressed', String(b.dataset.view === view)));
  const names = view === 'editor' ? ['MORPH', 'ATTACK', 'DECAY', 'UNASSIGNED'] : view === 'library' ? ['SELECT', 'UNASSIGNED', 'UNASSIGNED', 'UNASSIGNED'] : $('shift').checked ? ['MORPH', 'MOD', 'NOISE', 'CUTOFF'] : ['MORPH', 'PITCH', 'DECAY', 'BALANCE'];
  names.forEach((name, i) => { $(`name${i}`).textContent = `${tokens.encoderOrder[i].toUpperCase()} / ${name}`; });
  $('screen-caption').textContent = view === 'library' ? 'Tape Library is a design reference, with simulated selection and status. Tape Archive has no public implementation yet.' : 'Original reference artwork. Host preview; device rendering and case clearance require native checks.';
  const spec = scene();
  text(spec, { id: 'spec-small', value: 'SMALL / LABELS', x: 16, y: 16 });
  text(spec, { id: 'spec-medium', value: '18 / EDIT', x: 16, y: 44, size: 'medium' });
  text(spec, { id: 'spec-large', value: '28', x: 16, y: 88, size: 'large' });
  $('type-specimen').innerHTML = renderSvg(spec);
  if (font) applyStockFont($('type-specimen').querySelector('svg'), spec, font);
}
for (let i = 0; i < 4; i++) {
  const label = document.createElement('label');
  label.innerHTML = `<span class="slider-label"><span id="name${i}"></span><output id="value${i}">40</output></span><input id="slider${i}" type="range" min="0" max="100" value="40">`;
  label.querySelector('input').style.accentColor = tokens.color[tokens.encoderOrder[i]];
  label.querySelector('input').addEventListener('input', e => { values[i] = Number(e.target.value); $(`value${i}`).textContent = values[i]; if (view === 'library' && i === 0) selected = Math.min(2, Math.floor(values[i] / 34)); draw(); });
  $('sliders').append(label);
}
document.querySelectorAll('[data-view]').forEach(b => b.addEventListener('click', () => { view = b.dataset.view; draw(); }));
for (const id of ['zoom', 'guides', 'shift', 'browser-state']) $(id).addEventListener('change', draw);
$('key1').addEventListener('click', () => { view = view === 'play' ? 'editor' : 'play'; draw(); });
$('key2').addEventListener('click', () => { $('validation').textContent = view === 'play' ? 'T2 keeps the native envelope route; no envelope engine is simulated here.' : view === 'library' ? `Selected reference: ${['FIRST LIGHT', 'NIGHT BUS', 'SLOW WAVES'][selected]}. No real tape file is loaded.` : 'Previous-page action is an integration contract; this reference shows one editor page.'; });
$('key3').addEventListener('click', () => { if (view === 'library' && $('browser-state').value === 'error') { $('browser-state').value = 'ready'; draw(); $('validation').textContent = 'Simulated retry returned to the reference list.'; } else $('validation').textContent = view === 'library' ? 'LOAD is a layout contract. No tape-storage operation is implemented or performed.' : 'Native FX / next-page route must come from your plugin contract.'; });
$('key4').addEventListener('click', () => { $('validation').textContent = 'This reference has no text-entry or tape-tools backend. See the documented component contracts.'; });
async function setFont(source) {
  font = await loadStockFont(source);
  $('font-status').textContent = `Local stock geometry loaded. SHA-256 ${font.sha256}. Optical-spacing host profile; native kerning not reproduced.`;
  $('glyphs').textContent = `Text IDs: ${font.ids.join(' ')}\nLatin A–Z use IDs a–z. Uppercase source IDs are icons. Space is a layout advance.`;
  draw();
}
$('font-file').addEventListener('change', async e => { try { if (e.target.files[0]) await setFont(await e.target.files[0].text()); } catch (error) { $('font-status').textContent = error.message; } });
$('clear-font').addEventListener('click', () => { font = null; $('font-file').value = ''; $('font-status').textContent = 'Schematic alphabet. Not the stock OP-1 font.'; $('glyphs').textContent = 'Load a stock font to inspect its inventory.'; draw(); });
draw();
