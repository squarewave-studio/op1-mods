# Sources, provenance and limits

Source references, **20 October 2026**. Public material informs behaviour, constraints and
attribution. No TE code, firmware, glyph geometry, stock artwork or GPL SDK code
is included. The ledger identifies the scope and limits of each reference.

| Source | Reviewed identity | Use and limit |
| --- | --- | --- |
| [TE original OP-1 guide](https://teenage.engineering/guides/op-1/original) and [modules](https://teenage.engineering/products/op-1/original/modules) | Live original-device pages, accessed 2026-10-20 | Primary stock interaction and visual reference. Our design interpretation is not an official design system. Do not substitute the Field guide. |
| [op1hacks SVG notes](https://github.com/op1hacks/docs/blob/e904a4c2afca2a8cc6b0751a913d160fb46ad3aa/svg.md) and [font/symbol notes](https://github.com/op1hacks/docs/tree/e904a4c2afca2a8cc6b0751a913d160fb46ad3aa#font--symbols) | `e904a4c2afca2a8cc6b0751a913d160fb46ad3aa` | SVG restrictions, ID encoding, uppercase-looking text/lowercase IDs, icon namespace and kerning resource. Several older SVG tags are explicitly uncertain; this kit does not adopt those as qualified. |
| [op1repacker](https://github.com/op1hacks/op1repacker) | Public usage documentation, accessed 2026-10-20 | Owner-operated local unpack route. External optional setup tool, not a bundled dependency. No firmware downloads are mirrored here. |
| [op1r source](https://github.com/pcppcp/op1r-src/tree/358d7e6d0460318e1108784fe14acfbfd3850ab4) and [plugin guide](https://github.com/pcppcp/op1r-src/blob/358d7e6d0460318e1108784fe14acfbfd3850ab4/synth_sdk/PLUGINS.md) | `358d7e6d0460318e1108784fe14acfbfd3850ab4` | Plugin/screen separation, Q10, visibility and host harness behavior. SDK is GPL with its additional firmware permission; no SDK code copied. |
| [op1r font loader](https://github.com/pcppcp/op1r-src/blob/358d7e6d0460318e1108784fe14acfbfd3850ab4/synth_sdk/display/svgfont.py), [Oblique generator](https://github.com/pcppcp/op1r-src/blob/358d7e6d0460318e1108784fe14acfbfd3850ab4/synth_sdk/display/gen_oblique.py), [host renderer](https://github.com/pcppcp/op1r-src/blob/358d7e6d0460318e1108784fe14acfbfd3850ab4/synth_sdk/harness/render.py) | Same pin | Useful prior art: stroke glyph loading, escaped IDs, measured widths. Font-loader file attributes its MIT origin to Martin Mairinger / [ultralazr's Oblique Strategies](https://github.com/ultralazr/op1-oblique-strategies). Our loader is independently written. Host renderer's generic text stand-in does not qualify stock typography. |
| [Hundekraft workshop](https://hundekraft.github.io/op1-workshop/), [manual](https://hundekraft.github.io/op1-workshop/manual.html), [developer reference](https://hundekraft.github.io/op1-workshop/llms.txt) | Site 3.0.2; site repo tree `26e09fcb6a906449d4af7e6f7294af1c526690fc` | Current player/developer conventions, secondary readouts, screen constraints and core-specific caching. Versioned SDK is the reference for installed-core capabilities. |
| [Hundekraft 3.0.2 source/SDK](https://hundekraft.github.io/op1-workshop/source/hundekraft-3.0.2-source.tar.gz) and [published checksum](https://hundekraft.github.io/op1-workshop/source/hundekraft-3.0.2-source.tar.gz.sha256) | SHA-256 `35d481d4fd6e3d0e8699bf6fccd8e548c2929ca9155b82b909c175ef24de9700` | Checksum verified; read `ext/kennel/docs/screen-art.md` and API declarations. Restricted SVG, 12-unit effect correction and optional immutable-prefix pixel cache are framework facts, not universal hardware rules. No archive or code vendored. |
| [Maiko's OP-1 page](https://maikopruett.com/op-1), [guide](https://maikopruett.com/op-1/guide), [customizer](https://maikopruett.com/op-1/custom) | Live #250 pages, accessed 2026-10-20 | Tape context, naming, undo feedback, sampler and storage interactions. User-facing guidance/visual reference; no independently published reusable UI standard or separate Maiko SDK was established by this review. No art/code copied. |
| [Public GIN docs](https://github.com/squarewave-studio/op1-mods/blob/1465a935e22467dec85a7c918819891b000404e4/mods/gin/docs/CONTROLS.md), [screens](https://github.com/squarewave-studio/op1-mods/blob/1465a935e22467dec85a7c918819891b000404e4/mods/gin/README.md), [native UI source](https://github.com/joseph-holland/op1r-src/tree/ec5d6f722df05b77f80706db239352992937f4d7/synth_sdk/experimental/gin/standalone/src/tonic/ui_native) | op1-mods base `1465a935e22467dec85a7c918819891b000404e4`; native source `ec5d6f722df05b77f80706db239352992937f4d7` | Primary text-size choices and RGB values, explicit edit A/B and persistent morph lane. Existing source has additional text sizes and fallback punctuation; this proposal deliberately consolidates the roles. Host screenshots do not prove native execution. GPL native renderer is not imported. |
| [Tape Archive](https://github.com/squarewave-studio/op1-mods/blob/1465a935e22467dec85a7c918819891b000404e4/mods/tape-archive/README.md) | Same op1-mods base | Public placeholder; no released backend or public UI implementation to claim compatibility with. New browser layout is original reference artwork. Local earlier concepts informed discussion only; no private material published. |
| [Joe's playing example](https://op-forums.com/uploads/default/original/2X/e/e7fb7c193a878b2a7e11d16c2b51cad877e0eaa3.png) and [editor example](https://op-forums.com/uploads/default/original/2X/a/a8abb2d7f132b6eee717f22fba9b090fa0a2dce4.png) | Images supplied in the design brief | Three-role intent and physical lower-window concern. Reviewed, not copied into the kit. Screen clearance is a user observation, not a measured universal bezel spec. |
| [Browser SVG rendering hints](https://developer.mozilla.org/en-US/docs/Web/SVG/Reference/Attribute/shape-rendering) | Accessed 2026-10-20 | Host antialiasing behavior only. No evidence that CSS hints enable native OP-1 smoothing. |
| [Agent Skills specification](https://agentskills.io/specification) | Accessed 2026-10-20 | Portable skill folder, SKILL.md frontmatter and supporting references. |

## Evidence classification

- **Stock reference:** original-device interaction grammar and privately supplied
  font resource identity. No TE-provided three-size design token system was found.
- **Community-documented:** SVG restrictions, ID conventions, SDK services and
  specific screen offsets, at the versions/limits above.
- **Existing SquareWave practice:** GIN's main label/readout parameters and colours,
  morph lane, page controls, explicit edit state and host-preview disclosure.
- **Proposed library defaults:** three roles, 11/18/28 nominal targets, 8/8/8/16
  insets, spacing grid, secondary colour and original reference components.
- **Qualification:** host appearance, native rendering, physical legibility and
  CPU/audio deadline behavior are separate claims.

## Questions this version leaves open

1. Measure physical-size legibility and lower-window clearance across display
   revisions before claiming universal insets.
2. Qualify per-framework native Text size/ink/advance mappings and kerning, rather
   than treating the browser optical profile as native identity.
3. Establish a separately named, sourced stock-colour profile if exact native
   RGB/colour-resource identity is needed. Current tokens identify GIN values.
4. Measure raster edge counts and peak graphics/audio cost for representative
   device screens; no host metric substitutes for this.

These are research/adoption limits, not prerequisites for using the voluntary
design guidance or building original host layouts.
