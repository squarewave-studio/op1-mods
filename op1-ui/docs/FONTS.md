# Stock fonts and host previews

Use stock lettering for a preview described as faithful to the OP-1. An agent
must identify the font mode, source asset hash, metric/spacing profile and renderer.
Schematic lettering is useful for an asset-free public example, but does not
validate stock typography. This kit includes no TE font files or glyph geometry.

## Where `opfont.svg` comes from

The vector UI font is a resource in the owner's **locally unpacked original
OP-1 firmware**, at package-relative **`content/display/opfont.svg`**. It is not
a downloadable web font or a file normally visible in the OP-1's USB sounds/tape
disk. Source checkouts and the standalone Kennel SDK are not a reliable source
of the proprietary font. The boot/console bitmap text is a different font system.

Prefer an existing lawful local unpacked tree. Ask the user for its location if
it is not known. Check filenames locally rather than downloading somebody's
extracted font. For example:

```sh
rg --files --hidden --no-ignore /absolute/private/firmware-work \
  -g opfont.svg -g kerntable.db
```

If the user has no unpacked tree, explain the supported route: obtain their own
original OP-1 firmware from [TE's official downloads](https://teenage.engineering/downloads/op-1)
and follow [op1repacker's documented setup and unpack procedure](https://github.com/op1hacks/op1repacker#usage).
In a **separate private scratch directory**, with the tool already installed:

```sh
cd /absolute/private/firmware-work
op1repacker unpack /absolute/private/downloads/op1_246.op1
# Expected tree: op1_246/content/display/opfont.svg
# Related stock resource: op1_246/content/kerntable.db
```

Check the output path reported by the installed tool; do not assume the working
directory or stem if that version differs. Firmware acquisition, installing an
unpacker and unpacking are explicit setup operations, not hidden steps in the
preview command. No key, OTP or decryption work is required for this resource
workflow. This kit does not install tools, fetch firmware, unpack it, repack it
or operate a device. Do not run an unpacker in this public checkout.

Keep the font and its derived data outside the public repository. Use a private
absolute path. `OP1_FONT_PATH` is the portable configuration name, not a fixed
machine-specific installation location. `.gitignore` is a backstop, not a licence.

## Inventory before choosing labels

```sh
python3 op1-ui/tools/inspect_font.py "$OP1_FONT_PATH" \
  --text 'ATTACK 145 MS / EDIT A'
```

The tool prints the asset SHA-256, letters, digits, punctuation, icon IDs,
non-character IDs, transform presence and missing requested characters. It emits
no geometry. Missing characters cause a nonzero exit code. Re-run for the actual
labels, units, name character set, errors and confirmations in your screen.

Stock ID rules are documented by [op1hacks](https://github.com/op1hacks/docs#font--symbols):

- Text IDs `a`–`z` draw capital Latin letters. User text `ATTACK` maps to IDs
  `a t t a c k`; do not uppercase the SVG IDs.
- Uppercase IDs are UI symbols. For example `D` is the drum symbol and `Q` the
  sequencer symbol. They are a separate icon namespace, not capital-letter glyphs.
- Escaped IDs encode punctuation/digits: `_x23_` is `#`, `_x30_` is `0`,
  `_x3C_` is `<`. Decode the escape before measuring or validating characters.
- Some IDs name multi-character symbols; do not treat them as ordinary text.
- Space is normally a layout advance, not an outlined glyph. Native spacing
  involves `content/kerntable.db`; mere glyph presence does not supply kerning.

A local original-#246 metadata inventory during this work contained A–Z,
0–9 and punctuation `! " # % * + , - . / : ; < = > ? _`, plus uppercase icon IDs
`D Q X A L H E F S T M Y I J Z` and non-character ID `_x34_by4`.
This is a **metadata observation of one asset**, not a redistribution of it or
a promise for every firmware. Apostrophes, parentheses, brackets, degree signs,
µ and accented characters were not ordinary single-character text IDs in that
inventory. Never infer their presence from a renderer that draws replacements.
Use `US` if that is the chosen understandable unit label; label a custom glyph
adapter and its licence separately when a symbol is genuinely needed.

Report these independently: present in SVG → accepted by the adapter → drawn
by the native text route → readable at the selected size. GIN's working subset
and fallback punctuation are not a complete stock character catalogue.

## Load the font without publishing it

Open the optional local workbench (`op1-ui/workbench.html` via the local preview server) and choose your own `opfont.svg`
with **Load opfont.svg**. The browser reads the file into memory. Nothing is
uploaded. The kit never serves the font or fetches one from a CDN. There is no
font cache or font-bearing export button. Reloading clears the loaded font.
`OP1_FONT_PATH` is a suggested local variable for the metadata inspector, not
an HTTP route or hosting configuration.

For another browser host:

```js
import { loadStockFont, applyStockFont } from './src/stock-font.mjs';
const font = await loadStockFont(await chosenFile.text());
// svg is the DOM root returned from the reference screen's renderSvg output.
const issues = applyStockFont(svg, screenScene, font);
```

The loader strips a simple external DOCTYPE declaration without resolving it,
rejects entity declarations/internal DTDs and transforms, and rebuilds only
supported geometric elements with allowed geometry attributes. It does not
insert scripts, links, images, CSS or external references from the source.
Lowercase text glyphs are selected by ID, rather than loading the coloured icon
namespace into text. Unsupported revisions require an explicit adapter.

## Metric contract across hosts

Use role names `small`, `medium`, `large`, proposed nominal heights `11/18/28`,
top-of-cap-cell y coordinates and logical 320×160 units. Record these separately:

| Field | Meaning |
| --- | --- |
| Asset SHA-256 | Exact local font identity |
| Font mode | `stock-local` or `schematic`; never an unlabelled fallback |
| Nominal height | Shared role target, not a native constructor parameter |
| Ink bounds | Geometry bounds plus stroke/overhang, after placement |
| Advance | Cursor movement, which may be narrower than visible ink |
| Spacing profile | Native `kerntable.db`, qualified adapter, or named host approximation |
| Renderer/output | Browser/Cairo/native; logical canvas, scale, clipping and antialiasing |

This browser adapter uses a shared cap-cell height from the locally measured
alphabet, independent glyph widths, a minimum narrow-glyph advance of 0.22h,
inter-letter gap of 0.17h and space advance of 0.48h. It is called
**`stock-local / optical-v1`**. The geometry is stock; spacing is an explicit
host approximation inspired by GIN's published adapter. It does not read the
native kerning database or claim pixel identity with a native Text object.
If exact stock spacing is required, use a qualified native renderer or an
adapter for that local kerning resource and verify captured bounds. Do not
silently feed these nominal heights to a native size constructor.

Use actual stock measurement to re-check layout; schematic fixed-width checks
are insufficient. The workbench reports stock overflow separately. It does not
shrink overflowing text. A faithful native report still needs native captures.
Preserve a fixed-size editable-name window and visible cursor rather than
scaling a long name. Handle unsupported characters explicitly.

## Public-source boundary

Commit loaders, tests with invented glyphs, schema and metadata-only notes.
Do not commit `opfont.svg`, `kerntable.db`, extracted geometry, generated
`font.private.inc`, font atlases, baked stock-lettering SVGs or firmware.
Keep stock-font screenshots/private exports local unless their publication is
separately reviewed; the kit's committed examples use original schematic glyphs.
The MIT licence covers our loader, not the asset the user supplies.
