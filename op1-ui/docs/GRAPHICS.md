# Graphics, smooth edges and CPU

The display is a budget shared with an instrument. Prefer original static vector
art, bounded state changes and the smallest useful live layer. SVG is an authoring
format; it still becomes raster work on the device. Fewer file bytes do not
necessarily mean fewer raster edges or less CPU.

## Native SVG is a restricted profile

[op1hacks' SVG notes](https://github.com/op1hacks/docs/blob/e904a4c2afca2a8cc6b0751a913d160fb46ad3aa/svg.md)
and the [Kennel guide](https://hundekraft.github.io/op1-workshop/llms.txt) describe
format restrictions. This library chooses a conservative subset for generated
assets: SVG root, groups, paths, lines, circles, polylines and rectangles with
explicit paint. Paths use explicit `M`, `L`, `C`, `z` commands and at most four
decimal places. Repeat `L` for each coordinate pair. Normalize exponents and
all numeric attributes, not just path coordinates.

Do not ship browser `<text>`, CSS, filters, blur, gradients, embedded fonts,
scripts, external resources or raster `<image>` elements through this profile.
Use the target's text service for dynamic stock text. Convert quadratic curves
to cubics and flatten authoring transforms into coordinates. A qualified
framework may support specific matrix transforms; do not infer arbitrary group
transform support from a browser. The checker rejects transforms conservatively,
even where a particular SDK can safely apply a native transform at runtime.

Give each newly generated filled contour its own path. Disconnected filled
subpaths can join incorrectly in the native renderer. Keep style on the actual
shape, preserve paint order and do not merge every contour to save bytes.
The 8 KiB target is a community artwork budget, not the KNPL file-size limit or
a proof of safe per-path allocation. Long compound paths can fail independently
of total file size. The kit's path-text examples are larger; they are reference
layouts, not optimized loadable plugin assets.

## Named blocks and visibility

Split a composition into stable semantic groups, for example:

1. `base`: immutable background structure.
2. `idlepose` and `activepose`: complete prebuilt alternative states.
3. `live`: the small changing readout or indicator layer.
4. `front`: foreground rim/occlusion, drawn last when it must cover live content.

Use a node contract recording source ID, native normalized ID, expected type,
owner, initial visibility, source state, paint order and invalidation event.
Avoid IDs that collide after the target's normalization, including underscore
rewrites. Find/check every required node after document load. Missing or wrongly
typed nodes should select a deliberate degraded/error view, not dereference null.
Rebind after a replacement. Deletion or document reload invalidates handles;
hiding a group is not deletion.

Where the **matching SDK exports a group-visibility service**, turn whole unused
blocks off rather than hiding each primitive. op1r documents that its group flag
skips child traversal. Resolve once at init, then publish a visibility change
only when the logical state changes. Use SDK declarations and ownership rules;
do not copy raw addresses/structure offsets from this guide or another firmware.
Browser CSS `display:none` is not evidence of a native binding.

Pseudocode, not a portable SDK API:

```text
on_document_loaded:
    resolve and validate base, idlepose, activepose, live, front
    invalidate previous handles
    apply current state once
on_input_changed:
    choose idle or active; compare with published state
    if changed: publish visibility and update affected readouts
on_page_left:
    clear transient activity; retain model state
on_idle_timeout:
    return to idle once; stop cosmetic updates
```

Choose event feedback over continuous movement. Two complete poses can communicate
action without per-frame rotation, resampling, moving labels, particles or liquid
deformation. Keep event timing bounded, clear it on mode leave, and do not replay
stale motion when returning. This is a recommendation, not a ban on expressive
motion that earns its measured cost.

## Retaining geometry and caching pixels are different

| Technique | What it can avoid | What it does not establish |
| --- | --- | --- |
| Retained SVG nodes | Reparse, allocation, reconstruction | No guarantee the nodes stop rasterizing each frame |
| Update-on-change strings/transforms | Reformatting and repeated mutations | No guarantee unchanged paint is skipped |
| Hiding unused groups | Traversal/drawing of hidden children in a supporting runtime | No automatic cache for visible groups |
| Geometry/pose cache | Recomputing contours or transformed points | No automatic framebuffer/pixel reuse |
| Pixel cache | Re-rasterizing its valid cached area | Extra memory, composite cost and invalidation still matter |

Kennel 3.0.2 documents an optional immutable-top-level-prefix pixel cache.
It requires an always-visible prefix drawn first onto the cleared raster;
mutable children start at a known boundary, retain paint order and stay outside
that prefix. Use it only when the installed core exports the service and its
memory/lifetime contract applies. It can fall back to ordinary rendering.
It is not a generic OP-1 capability, GPU assumption or permission to preserve an
arbitrary framebuffer. Layer interleaving can make a single prefix unsuitable.

Cache strings, measurements and stable geometry on changes. Do not rebuild all
SVG nodes or draw an arbitrary UI from an audio callback/ISR. Keep updates in
the documented graphics owner. Publish a coherent snapshot; visible values do
not acknowledge DSP, I/O or persistent-save completion. Respect the framework's
document lifetime and drawing-cache cleanup rules.

## Smooth edges without hiding costs

Spatial antialiasing, curve approximation, temporal motion smoothing and audio
parameter smoothing are different problems. Do not fix one by changing another.

- Preserve useful cubic curves; coarse line approximations can look angular.
  Simplify nearly straight geometry with an explicit error tolerance measured
  at output size, not by dropping every other point. Avoid long duplicate or
  zero-length segments and excessive tessellation.
- Preserve fractional geometry where the renderer supports it, including Q10
  subpixel coordinates in matching SDKs. Rounding every point to an integer can
  create stair steps. A browser half-pixel adjustment is not universally correct
  after scaling to physical pixels.
- Inspect 0.85–1.5 unit strokes, joins, line caps, diagonals and curve extremes
  at final size. More width or rounded joins may change the design and native
  cost. op1r's host harness explicitly uses butt caps for procedural raster lines.
- For browser SVG, use ordinary antialiasing/geometric precision for curves.
  Avoid global `crispEdges`/`optimizeSpeed`: they can disable antialiasing.
  [Browser rendering hints](https://developer.mozilla.org/en-US/docs/Web/SVG/Reference/Attribute/shape-rendering)
  are host settings, not proven native SVG features.
- Rasterize host images at their final intended output resolution. An optional
  supersampled/downsampled design view can help inspect curves, but label it and
  keep a direct final-resolution comparison. Nearest-neighbor enlargement is
  useful to examine existing pixels, not to demonstrate smooth on-device edges.
  Do not put a blur/glow filter or high-resolution raster workaround into the
  device pipeline without separate support and cost evidence.
- For optional event motion, preserve time-based state and stop at rest; do not
  increase GUI frame rate or continuously recompute a cosmetic interpolation to
  conceal spatial aliasing. Control/audio smoothing belongs to its own owner.

The workbench keeps vector curves and browser antialiasing. Its “400×200
comparison” is a scaled logical view, **not a native 400×198 capture**. Native
coverage, clipping, physical OLED visibility and timing need their own checks.
There is no universal antialiasing flag established by this library.

## Measure the right thing

Record total/source SVG bytes, path/node count, active and hidden blocks, mutable
nodes, curve/edge counts, redraw frequency, allocations and cache memory. Then
measure native elapsed/peak graphics CPU and combined audio deadline behavior
under maximum voices/FX and input activity. Separate idle and active screens.
Host call counts and elapsed host time are useful diagnostics, not hardware CPU
percentages. State the renderer/core version and output dimensions. This kit
claims no measured native performance improvement.
