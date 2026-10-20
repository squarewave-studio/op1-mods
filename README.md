# OP-1 Mods By SquareWave Studio

I still make music on the original OP-1, so I've started building new things
for it. First up is GIN, a drum synth.

## OP-1 UI: A Shared Design Kit

Building a synth, effect or tool? [OP-1 UI](op1-ui/README.md) proposes a shared
Small/Medium/Large text scale, four encoder colour roles, screen margins and
practical SVG/performance guidance. It includes reusable components, an
optional local preview and the [op1-design skill](skills/op1-design/SKILL.md).
Install it in a firmware project with `npx skills add squarewave-studio/op1-mods`.
[Screen Examples](op1-ui/docs/EXAMPLES.md) show original synth, sampler, envelope,
FX, sequencer, tape and mixer layouts informed by stock #246 display resources.
The static guide needs no font setup. Faithful local previews use your own
`opfont.svg`; no TE font or firmware is included. [Sources and evidence limits](op1-ui/docs/SOURCES.md) ·
[Contribute](CONTRIBUTING.md) · [MIT licence](LICENSE).

## GIN: Synth Drums On All 24 Keys

[![Watch the GIN demo on YouTube](https://img.youtube.com/vi/YZD0u1V243E/hqdefault.jpg)](https://www.youtube.com/watch?v=YZD0u1V243E)

GIN turns the keyboard into a synth drum kit, based on the PO-32 Tonic's drum
engine and built on Eric Lewis's [libpo32](https://github.com/ericlewis/libpo32).
Every key is its own sound, and every sound is synthesised, not sampled.

Each key holds two versions of its sound, A and B. Turn the blue encoder to
morph between them while you play, so a kick can go from tight to boomy
halfway through a pattern. Green, white and orange give you pitch, decay and
the tone and noise balance straight away. Hold shift for pitch modulation,
noise decay and the noise filter.

If you want to go deeper, T1 opens the Advanced pages with all 21 parameters,
including separate tone and noise envelopes, EQ and velocity. You can name
each sound, and your edits are still there after a restart.

It comes with eight kits adapted from CNTR RNDM's free PO-32 kits, each one
laid out across all 24 keys.

![GIN play screen, rendered on the host](mods/gin/docs/screenshots/play.png)

*Rendered from the screen code with the original OP-1 font. It isn't a photo
of the hardware or a capture of GIN loaded through OP1P.*

### Where It's At

GIN 1.0.0 is a source release for community firmware builders, tested on the
original OP-1 with firmware #246. The reusable synthesis core is here, and the
native instrument and build source are in the linked op1r fork. There's no
loadable `gin.op1p` yet. That needs a drum synth adaptor for op1r, which I'm
working on next.

[Get The Source](mods/gin/docs/BUILDERS.md) · [Read About GIN](mods/gin/README.md) · [Compatibility](mods/gin/docs/COMPATIBILITY.md)

## More Mods On The Way

Each mod gets its own folder, docs and release tags. GIN uses `gin/v1.0.0`.
Orbital and Tape Archive have folders ready, with no releases yet.

| Folder | Mod |
| --- | --- |
| [mods/gin](mods/gin/README.md) | Drum synthesis |
| [mods/orbital](mods/orbital/README.md) | Sequencing |
| [mods/tape-archive](mods/tape-archive/README.md) | Tape storage workflow |
| [op1-ui](op1-ui/README.md) | Shared UI guidance, components and agent skill |

## Licence And Credits

GIN's synthesis core derives from Eric Lewis's MIT-licensed
[libpo32](https://github.com/ericlewis/libpo32). The factory kits adapt eight
of [CNTR RNDM's free PO-32 kits](https://youtube.com/playlist?list=PLlkIAiu0_jPw3DgmZm1RbmFe86rzuWU2o).
op1r is by pcppcp and contributors. Thanks to all of them.

The [MIT licence](LICENSE) covers the original reusable code and
documentation in this repository. Third-party work keeps its own terms. GPL
host adapters and loader changes are kept separately in the op1r
contribution, so a combined plugin binary isn't automatically MIT-only. See
[THIRD_PARTY.md](THIRD_PARTY.md).

These are mods by [SquareWave Studio](https://squarewave.studio).
Unofficial. Not affiliated with, authorised by or endorsed by Teenage Engineering AB.
Not affiliated with Sonic Charge / NuEdge Development AB. All trademarks belong
to their respective owners. See the [ownership and source-publication notes](THIRD_PARTY.md#independence-and-trademarks).

## Support My Open Source Work

These mods are free. If one has been useful, you can leave a thank-you tip. It
doesn't unlock anything. It just helps me keep building them.

- [Buy me a coffee ($5)](https://buy.squarewave.studio/checkout/buy/880667af-df68-45e1-b8e5-e36c2b5de26e)
- [Buy me lunch ($10)](https://buy.squarewave.studio/checkout/buy/eeb321a3-dea8-486d-9ef7-0794584feb9c)
- [Buy me dinner ($20)](https://buy.squarewave.studio/checkout/buy/edba5162-42c0-4c27-bc95-f41096c2451d)
- [Choose your own amount](https://buy.squarewave.studio/checkout/buy/59446300-c1a9-43d5-90ce-6b4c9f4252c7) (from $3)

I also make [OP-PatchStudio](https://squarewave.studio/op-patchstudio) for the
OP-1 Field and OP-XY.
