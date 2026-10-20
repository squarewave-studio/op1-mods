# Stock Screen Patterns

The original OP-1 makes a musical relationship visible: a wave has boundaries,
an envelope has stages, a mixer has four lanes and a sequencer separates notes
from playback position. Use that relationship as the centre of a screen. Add
readouts where they help the player understand or set a value.

## What Was Inspected

All **61 SVG resources** in an owner's locally unpacked stock **#246**
`content/display` directory were checked against an existing SHA-256 inventory.
This includes `colors.svg` and `opfont.svg`. The remaining 59 resources were
rendered privately to inspect their visual vocabulary. Nothing extracted is
included or served here. This census covers the packaged SVG resources, not
every native page, state, animation or firmware revision.

A stock SVG is often a set of templates and alternative states. Rendering the
whole file can show overlapping labels, poses and placeholders that native code
would select, replace, move or hide. Empty-looking resources are not evidence of
an empty native screen. Use the census to understand composition and resource
structure; use matching native bindings or captures to establish actual runtime
layout and behaviour. A hash match proves resource identity, not visibility.

To inspect your own tree without exporting paths or glyphs:

```sh
python3 op1-ui/tools/audit_stock.py /absolute/private/path/content/display
```

The report includes hashes, element counts, declared viewBoxes and literal
paint colours. It reads local files, makes no network requests and writes no
asset files. Keep your resource tree outside this repository and public output.

## Screen Families And Design Lessons

| Stock resources inspected | Useful design relationship | Original kit example |
| --- | --- | --- |
| `fm`, `t10`, `drw`, `st`, `cls`, `pls`, `pd`, `ptch` | A recognisable instrument or signal relationship, with encoder-owned features | Synth Relationships |
| `adsr`, `dynaenv`, `playmode`, `tune` | Shape or setting rows explain how a note behaves | Envelope Stages |
| `sampler`, `drum2` | Whole-sample context, detailed waveform and distinct boundaries | Sample Boundaries |
| `dbox`, `dsynth` | Selected voice, tone and envelope belong together | Drum Selection |
| `delay`, `grid`, `mllp`, `mmmf`, `ftwo`, `bode`, `rymd` | Each effect has a visual metaphor tied to its parameters | Effect Relationships |
| `duallfo`, `singlelfo`, `rndlfo`, `reroutelfo`, `midilfo`, `bendlfo`, `cranklfo` | Source, amount and destination form a modulation route | Modulation Routing |
| `slump`, `iter`, `endless`, `tombola`, `pattern`, `simple`, `sketch`, `etchasketch` | Musical events and playback position need distinct cues | Step And Play Position |
| `tape`, `tapeconfig` | Time, tracks, transport and recording state remain legible | Tape Position |
| `mixer` | Four physical lanes retain their positions and ownership | Four Control Lanes |
| `eq` | Frequency regions share a neutral reference | Frequency Regions |
| `master` | Transfer behaviour and live level are different kinds of feedback | Dynamics Feedback |
| `tempo` | One primary number dominates supporting settings | One Dominant Value |
| `micline`, `in` | Input source, level and recording state are explicit | Input And Level |
| `album` | Side, elapsed time and record/stop state belong together | Recording State |
| `signalflow` | Routing and activity must be distinguishable | Modulation Routing |
| `presetbrowser`, `clock`, `com`, `ok`, `save`, `octave`, `fmpopup`, `subscreenhand`, `help`, `lander`, `id` | Context, actions and status need their own hierarchy | No matching product or Browse page is proposed |

Resource names above omit `.svg`. This table accounts for the 59 non-font,
non-palette resources. Names group related visual lessons; they do not prove
that every resource is a selectable page or a runtime feature with that name.

The [Screen Examples](https://squarewave-studio.github.io/op1-mods/examples.html) are independent compositions and geometry.
They demonstrate proposed Small/Medium/Large roles and conservative clearances.
They are schematic code renders, not replicas, stock-font samples or hardware
captures. The kit does not prescribe a single alignment for every stock screen:
choose a consistent edge for related names and readouts, group values with units
and preserve the four encoder lanes where four columns are used.

## Colour Resource Identity

`colors.svg` SHA-256:
`f76b8b66cbb37eb141fbb7d14d8a624e373df8a080f2d51dc769c16881f9c1e5`.

The profile `stock-246-resource` uses Blue `#698eff`, Green `#00ed95`,
White `#ffffff` and Orange `#ff423f`. These are the named resource entries,
not calibrated OLED colours or a universal rule for all stock art. Static
`mixer.svg` and `sampler.svg` use `#ff3a5d`; `eq.svg` includes `#dfd9ff` and
`#aeb1dc`. The forum's `#4d9eff` Blue differs from this #246 entry. Keep a
community palette separately named rather than silently changing profiles.

The commonly declared artwork stroke width is 1.5 logical units; 2 also occurs.
That is a source observation, not a minimum enforced by the renderer. Declared
viewBoxes vary too: most are `0 0 320 160`, while the palette and input-resource
artwork include other authoring dimensions. New kit compositions deliberately
use the common 320×160 logical space, not an inferred physical panel resolution.

## Forum Reports And Working Guides

[maxaw, post 1371](https://op-forums.com/t/custom-firmware-on-the-op-1/4283/1371)
reports 147 units of usable height and suggests an 8-unit grid with
8/8/16/8 top/right/bottom/left insets. [Instant_Fidelity, post 1373](https://op-forums.com/t/custom-firmware-on-the-op-1/4283/1373)
recommends keeping content above about y=140 and records practical SVG, text and
drawing observations. These reports motivate the kit's cautious critical-ink
line at y=140 within the default safe rectangle ending at y=144.

Neither report establishes a universal bezel boundary or safe CPU budget.
Roughly 150 raster lines per frame is an attributed workload heuristic, not a
renderer limit: line length, width, edge count, clipping, text and SVG work also
matter. The [Graphics](GRAPHICS.md) guidance explains how to test those costs and
keep text and retained blocks from doing unnecessary update work.
