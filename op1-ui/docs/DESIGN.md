# Design Specification · Draft 0.1

## An Instrument First

Our interpretation of the original TE screens is an economical visual grammar:
black space, fine outlined objects, a small meaningful colour set, large readable
feedback and a picture that explains a musical relationship. A waveform, reel,
string or envelope earns its space when it makes the control easier to understand.
Use original artwork. Copying a stock drawing is unnecessary to follow this grammar.

Keep the playing surface immediate. Put detailed editing on clearly named pages.
Show what is selected and what the next encoder movement will change. Prefer
an understandable metaphor and a few controls over a miniature desktop panel.
These are design interpretations informed by the stock #246 display resources,
not published TE design-system rules. See [Stock Screen Patterns](STOCK.md)
for the resource census, original examples and the limits of static artwork.

## Three Text Roles

| Role | Proposed nominal height, logical px | Use |
| --- | ---: | --- |
| Small | 11 | Parameter names, units, context, status detail, T-key captions |
| Medium | 18 | Advanced editor values, selected list entries, short status messages |
| Large | 28 | Primary values while playing, a single dominant readout |

These numbers derive from GIN's primary label/readout choices, not a measured
three-size stock TE taxonomy. Stock native text size, stock-glyph adapters and
browser CSS size are different units. Measure actual stroked ink, bearings and
advance for the selected renderer. The reference renderer constructs these
heights directly; the stock host adapter scales a shared glyph cell.

Use only these roles in ordinary player-facing UI. Treat Small as the floor,
including units, hints and warnings. If a value or label does not fit: format
the value, use a familiar shorter name, move the unit, wrap onto another line,
use a fixed-size scrolling name window, or create a page. Do not auto-shrink,
stretch, squeeze or crop critical text. A missing glyph is an explicit error.
For example, prefer `1.2 S` over an unreadable long millisecond value, but
preserve the precision the user needs for the task.

## Four Encoder Lanes

Blue, Green, White, Orange, in physical order. Keep the label, edited value and
corresponding visual feature associated with the same lane colour. Labels,
position and shape should also identify ownership, so the UI remains usable
without colour discrimination. The fourth control is called Orange here;
older op1r documentation sometimes calls it Red. The #246 colour-resource RGB for that
role is red-orange; it does not change its physical identity.

Primary and Shift control maps should be documented side by side. Keep an
important persistent lane stable across pages where practical: GIN's morph is
an example. Show the secondary label, value and unit while it is active.
Do not invent touch, encoder push or Shift-fine behavior for a framework that
has not established that input contract.

At instrument level, respect the original T1 engine / T2 envelope / T3 FX / T4
LFO organization unless the plugin deliberately documents a different route.
In subpages, align four action captions with T1–T4; captions describe actual
actions. Preserve key-release bookkeeping when changing modes. A picture of
an action does not implement its input route.

## Alignment And Readout Groups

Choose one anchor for a parameter name and its value. In the reference Play
layout, left-side blocks share x=9 and right-side blocks share a right edge at
x=311. Do not left-align names while centring or independently shifting values.
In the reference editor, use stable column left anchors 9, 89, 169 and 249,
corresponding to Blue, Green, White and Orange. These are layout proposals.
Keep a value and its unit together on one baseline, measure the complete pair
and preserve its group anchor when switching font profiles. Do not place units
using the schematic number's width after loading a different stock font.

## Coordinates And The Window

Author in **320×160 logical units**. This is not a statement that the physical
OLED has 320×160 pixels. Original-device work uses a **400×198 output**; a
uniform 1.25 host scale yields 400×200 before any clipping. Treat mapping,
renderer clipping and case/window occlusion as distinct concerns.

Proposed content insets are Left/Right/Top **8**, Bottom **16** logical units.
The resulting safe rectangle is `x=[8,312]`, `y=[8,144]`. Include stroke extents,
text overhang, animation extremes, unit baselines and descenders within it.
Full-screen clears/veils may intentionally cover the whole canvas. The example
footer begins at y=128, keeping its 11-unit glyph and stroke above y=140.

[maxaw reports 147 units of usable height](https://op-forums.com/t/custom-firmware-on-the-op-1/4283/1371).
[Instant_Fidelity recommends keeping content above about y=140](https://op-forums.com/t/custom-firmware-on-the-op-1/4283/1373).
These are attributed device observations, not a universal clipping boundary.
Keep critical text and controls, including stroke extents, at or above y=140 as
a cautious working guide. The default safe rectangle still ends at y=144; its
last four units are clearance, not a target for important content. Kennel's
12-unit effect composition lift is a separate framework correction. Physical
display revisions and the case/window still need checks.
Do not add a framework's translation twice. Move artwork, text, pivots and
clipping together, while keeping full-screen veils full-screen.

Use an 8-unit layout grid with 4-unit subdivisions: 4 between related items,
8 between groups, 12 between sections. This adopts maxaw’s grid suggestion.
Optical alignment and measured glyph bounds can override the grid. These are
library proposals, not stock measurements.

## Palette And Strokes

| Role | Reference RGB | Status |
| --- | --- | --- |
| Background | `#000000` | Stock #246 colour resource |
| Blue | `#698eff` | Stock #246 colour resource |
| Green | `#00ed95` | Stock #246 colour resource |
| White | `#ffffff` | Stock #246 colour resource |
| Orange | `#ff423f` | Stock #246 colour resource |
| Secondary | `#b9b9c9` | Original library choice for readable secondary text |
| Decorative | `#625d77` | GIN reference dim tone; nonessential decoration only |

The named `stock-246-resource` profile takes the four encoder values from a
locally inspected stock `colors.svg`. It is not OLED colour calibration. Static
stock artwork uses variants: mixer and sampler include `#ff3a5d`, while EQ
includes `#dfd9ff` and `#aeb1dc`. The forum profile’s `#4d9eff` Blue is not the
Blue entry in this #246 resource. Do not silently combine profiles or claim one
universal palette across firmware and screens. Secondary and Decorative are
kit choices. Never use dim tones for essential Small text or make a warning
depend only on red/orange. [Stock evidence](STOCK.md) records hashes.

Use centred strokes and include half the width on each side when measuring
ink bounds. For a 1.5-unit line at x=8, the outer ink reaches x=7.25. There is
no portable inside/outside stroke setting in this native profile; inset the
centreline yourself. Use 1.5 units for ordinary artwork and 2 for emphasis,
reflecting common widths in the inspected stock resources. Text uses 0.85–1
unit strokes as proposed for these nominal role sizes, not as a measured stock
minimum. Avoid functional hairlines thinner than 1 without target checks.
Check curves, diagonals and fractional positions
at actual size; thicker is not automatically clearer. See [Graphics](GRAPHICS.md)
for renderer-specific antialiasing and why raster supersampling is a host tool.

## State Communicates Truth

Distinguish selected item, playing item, edit endpoint A/B, morph position,
draft name, applied name and saved state. Show a live modulation value only
when that is the intended readout; do not label a raw saved word as audible state.
Use the framework's documented normalized-live services where available.

Cancel discards a draft. Apply acknowledges the specific submitted change.
An on-screen value is not proof of a successful save. Keep a readable pending
or retry state until the backend reports completion. Show only recovery actions
that exist. Empty, loading, unavailable and failed states need different messages.
Before a destructive commitment, show the complete readable target and action;
a successful width check alone does not prove the native warning was rendered.

## Design Checks

Review minimum/default/maximum values, long names, unit changes, missing glyphs,
Shift, all pages, mode return, stale selection, empty/loading/error states and
save/cancel/retry. Inspect 1× logical and physical-output-size views plus enlarged
edges. A host screenshot is a design check. Native rendering, physical legibility
and peak CPU/audio behavior are separate qualification steps.
