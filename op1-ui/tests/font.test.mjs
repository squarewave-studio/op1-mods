import test from 'node:test';
import assert from 'node:assert/strict';
import { decodeGlyphId, textGlyphId, stockTextLayout } from '../src/stock-font.mjs';

test('stock text IDs preserve the distinct icon namespace', () => {
  assert.equal(textGlyphId('A'), 'a');
  assert.equal(decodeGlyphId('D'), 'D');
  assert.equal(decodeGlyphId('_x23_'), '#');
  assert.equal(decodeGlyphId('_x30_'), '0');
  assert.equal(decodeGlyphId('_x34_by4'), '4by4');
});
test('stock spacing uses independent ink width, explicit spaces and no missing-glyph substitution', () => {
  // Invented metadata, not extracted font geometry.
  const glyphs = new Map([['a', { box: { width: 5, height: 10 } }], ['i', { box: { width: 1, height: 10 } }]]);
  const font = { glyphs, capHeight: 10 };
  assert(stockTextLayout(font, 'A', 18).width > stockTextLayout(font, 'I', 18).width);
  assert(stockTextLayout(font, 'A A', 18).width > stockTextLayout(font, 'AA', 18).width);
  assert.throws(() => stockTextLayout(font, 'AB', 18), /missing 'B'/);
});
