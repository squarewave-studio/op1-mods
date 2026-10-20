---
name: op1-design
description: Design and review original OP-1 synth, effect and utility screens, including local stock-font host previews, supported SVG blocks and rendering-cost guidance. OP-1 Field and OP-XY need separate device profiles.
license: MIT
metadata:
  author: SquareWave Studio
  version: "0.1.0"
---

# OP-1 Design Kit

A draft design kit for original OP-1 screens. This file owns the shared rules:
the Spec page, design-kit.json and code constants are built from it. The
recommendations are voluntary and versioned, not a TE specification.

For stock composition and original examples read [Stock Screen Patterns](references/STOCK.md).
For typography read [Fonts](references/FONTS.md). For SVG, visibility, smoothing
or CPU read [Graphics](references/GRAPHICS.md). [Sources](references/SOURCES.md)
records public references and their limits. Use the matching framework's actual
input and graphics contract; do not infer runtime capability from a host image.

Choose controls/framework → font mode → layout/encoder ownership → original
components → complete text/SVG checks → final-size host inspection → separate
native/physical/performance qualification.

The local vector UI font is package-relative `content/display/opfont.svg`,
not a bundled web font. Prefer the user's existing private unpacked tree.
FONTS.md gives the official-firmware/local-unpack setup if it is missing. Run
`python3 scripts/inspect_font.py /absolute/private/path/opfont.svg --text 'ALL LABELS'`
from this skill folder. Text A uses SVG ID a; uppercase source IDs are UI icons.
Keep the font local. A browser file picker loads it into memory without uploading
or serving it. Do not put stock glyph geometry into public SVG exports.

The schematic reference font is explicitly separate. Stock-local/optical-v1 uses
stock curves with approximate spacing, not native Text kerning. Missing glyphs
and stock overflow need explicit handling; never validate stock text with the
schematic font's fixed-width measurement.

<!-- spec:start -->
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
not published TE design-system rules. See [Stock Screen Patterns](references/STOCK.md)
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
depend only on red/orange. [Stock evidence](references/STOCK.md) records hashes.

Use centred strokes and include half the width on each side when measuring
ink bounds. For a 1.5-unit line at x=8, the outer ink reaches x=7.25. There is
no portable inside/outside stroke setting in this native profile; inset the
centreline yourself. Use 1.5 units for ordinary artwork and 2 for emphasis,
reflecting common widths in the inspected stock resources. Text uses 0.85–1
unit strokes as proposed for these nominal role sizes, not as a measured stock
minimum. Avoid functional hairlines thinner than 1 without target checks.
Check curves, diagonals and fractional positions
at actual size; thicker is not automatically clearer. See [Graphics](references/GRAPHICS.md)
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
<!-- spec:end -->

## Machine-Readable Rules

This JSON block is canonical. Build derived files with `op1-ui/tools/sync_tokens.py`.

```json
{
  "schemaVersion": 1,
  "name": "OP-1 Design Kit",
  "version": "0.1.0",
  "status": "draft",
  "source": "skills/op1-design/SKILL.md",
  "tokens": {
    "version": "0.1.0",
    "status": "draft",
    "target": "original-op1",
    "coordinateSpace": {
      "width": 320,
      "height": 160,
      "unit": "logical-pixel"
    },
    "safeArea": {
      "left": 8,
      "top": 8,
      "right": 8,
      "bottom": 16,
      "criticalContentBottom": 140,
      "status": "proposed-default-not-measured-bezel"
    },
    "type": {
      "small": {
        "nominalHeight": 11,
        "lineHeight": 16,
        "strokeWidth": 0.85,
        "role": "labels, units, context and soft keys"
      },
      "medium": {
        "nominalHeight": 18,
        "lineHeight": 24,
        "strokeWidth": 1,
        "role": "editor values and selected list entries"
      },
      "large": {
        "nominalHeight": 28,
        "lineHeight": 36,
        "strokeWidth": 1,
        "role": "primary performance values"
      }
    },
    "color": {
      "background": "#000000",
      "blue": "#698eff",
      "green": "#00ed95",
      "white": "#ffffff",
      "orange": "#ff423f",
      "secondary": "#b9b9c9",
      "decorative": "#625d77"
    },
    "encoderOrder": [
      "blue",
      "green",
      "white",
      "orange"
    ],
    "space": {
      "layoutGrid": 8,
      "unit": 4,
      "related": 4,
      "group": 8,
      "section": 12
    },
    "stroke": {
      "fine": 0.85,
      "normal": 1.5,
      "emphasis": 2,
      "alignment": "center",
      "status": "artwork-defaults-stock-observed-not-a-renderer-minimum"
    },
    "artwork": {
      "targetBytes": 8192,
      "maxDecimals": 4,
      "status": "community-budget-not-renderer-limit"
    },
    "provenance": {
      "type": "Three roles proposed by Joe; nominal primary text heights observed in public GIN source, not verified TE ink metrics or an official stock scale.",
      "color": "stock-246-resource: four encoder entries from locally inspected colors.svg; secondary/decorative are kit choices. Static stock art has variants. Not OLED calibration.",
      "safeArea": "Proposed 8/8/16/8 top/right/bottom/left insets; maxaw reports 147 usable units and Instant_Fidelity suggests critical content above y140. Neither is a universal measured boundary."
    },
    "layout": {
      "encoderColumnLeft": [
        9,
        89,
        169,
        249
      ],
      "playLeftAnchor": 9,
      "playRightAnchor": 311,
      "valueUnitGap": 4,
      "status": "proposed-reference-layout"
    },
    "colorProfile": {
      "id": "stock-246-resource",
      "firmware": "246",
      "encoderSource": "content/display/colors.svg",
      "sha256": "f76b8b66cbb37eb141fbb7d14d8a624e373df8a080f2d51dc769c16881f9c1e5",
      "scope": "Four encoder entries only; secondary and decorative are kit choices, not OLED calibration."
    }
  },
  "rules": [
    {
      "id": "type-roles",
      "guidance": "Use Small, Medium or Large; shorten, format, wrap or paginate instead of shrinking below Small."
    },
    {
      "id": "font-provenance",
      "guidance": "Faithful host typography uses a local stock font with its hash, available glyphs and named spacing profile; schematic mode cannot validate stock typography."
    },
    {
      "id": "glyph-ids",
      "guidance": "Map Latin text to lowercase SVG IDs; uppercase IDs are icons. Reject missing glyphs explicitly."
    },
    {
      "id": "layout",
      "guidance": "Author at 320x160 logical units on an 8-unit grid with 4-unit subdivisions. Use proposed top/right/bottom/left insets 8/8/16/8 and keep critical ink at or above y140; distinguish physical output and framework offsets."
    },
    {
      "id": "encoder-ownership",
      "guidance": "Use Blue, Green, White and Orange in physical order, reinforced by labels and positions; document normal, Shift and page maps."
    },
    {
      "id": "alignment",
      "guidance": "Parameter names and values share a deliberate edge anchor; editor columns preserve encoder order. Measure value and unit as one baseline-aligned group in the selected font."
    },
    {
      "id": "state-truth",
      "guidance": "Keep selected, current, draft, applied and saved states distinct; show complete destructive targets and only real recovery actions."
    },
    {
      "id": "svg-profile",
      "guidance": "Use original explicit supported SVG geometry, stable checked IDs and separate filled contours; no browser-only features in native assets."
    },
    {
      "id": "retained-blocks",
      "guidance": "Prefer prebuilt named blocks, visibility changes and small live layers; publish mutations only when state changes and stop cosmetic updates at idle."
    },
    {
      "id": "cache-cost",
      "guidance": "Retained geometry is not pixel caching. Use pixel caches only where the exact runtime supports their lifetime and paint-order contract."
    },
    {
      "id": "smooth-edges",
      "guidance": "Separate antialiasing, curve tessellation and temporal smoothing. Preserve useful curves and subpixel geometry; host smoothing does not qualify native output."
    },
    {
      "id": "cpu",
      "guidance": "Measure native peak GUI/audio deadline cost separately from host counts; keep graphics out of audio and interrupt callbacks."
    },
    {
      "id": "assets",
      "guidance": "Keep TE code, firmware, fonts, extracted geometry, kerning databases and stock artwork out of public source and hosted files."
    }
  ]
}
```
