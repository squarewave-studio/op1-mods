# GIN

Designing your own instrument screen? See the [OP-1 Design Kit](https://github.com/squarewave-studio/op1-design-kit) for the shared UI guidance.

Synth drums on all 24 keys of the original OP-1, by [SquareWave Studio](https://squarewave.studio).

[![Watch the GIN demo on YouTube](https://img.youtube.com/vi/YZD0u1V243E/hqdefault.jpg)](https://www.youtube.com/watch?v=YZD0u1V243E)

GIN is a drum synth based on the PO-32 Tonic's drum engine, built on Eric
Lewis's [libpo](https://github.com/ericlewis/libpo). Every key is its own
sound, synthesised rather than sampled, so you can bend a kick or a hat into
something new without ever loading a file.

![GIN Play screen](docs/screenshots/play.png)

## Play It

Each key has two versions of its sound, A and B. The blue encoder morphs
between them, so you can push a snare from dry to splashy while a pattern
runs. Click blue to choose which side you're editing.

| Encoder | Turn | Shift + Turn |
| --- | --- | --- |
| Blue | Morph, A to B | Morph, A to B |
| Green | Tone pitch | Pitch modulation amount |
| White | Tone decay | Noise decay |
| Orange | Tone and noise balance | Noise filter cutoff |

Four voices play at once, with retriggering and voice stealing.

## Go Deeper

The engine has a lot more in it than four encoders can show. Press T1 for
eight Advanced pages covering all 21 parameters: tone, tone envelope, pitch
mod, noise filter, noise envelope, mix and level, EQ and velocity. The pages
draw the waves, envelopes and filter, so you can see what you're changing.

You can name every sound, up to 23 characters. Edits save to the working slot
and are still there after a restart. Hold one of the 1 to 8 snapshot buttons to keep a
separate named version of the whole kit.

## Eight Kits To Start With

GIN comes with eight factory kits adapted from
[CNTR RNDM's free PO-32 kits](https://youtube.com/playlist?list=PLlkIAiu0_jPw3DgmZm1RbmFe86rzuWU2o).
Each one fills all 24 keys. The original sounds are his. I've laid them out
across the keyboard and added variations.

## Screens

These host renders illustrate the original native UI, using the OP-1 font
loaded locally. They are not hardware photos or captures of a packaged plugin.
Font data is not included in this source release.

### Tone Envelope

![Separate tone attack and decay](docs/screenshots/tone-envelope.png)

### Noise Filter

![Noise filter controls](docs/screenshots/noise-filter.png)

### Mix And Level

![Mix and level controls](docs/screenshots/mix-and-level.png)

### Name A Sound

![Sound naming](docs/screenshots/name-sound.png)

### About

![GIN credits](docs/screenshots/about.png)

## Convert Your Own Kits

Use the [Gin kit converter](https://squarewave.studio/gin/) to convert your own
PO-32 transfers or Microtonic patches. The page explains how to arrange the
sounds and install the resulting preset on compatible Gin-enabled firmware.
The core source alone is not a firmware installer.

## Version 1.0.1

GIN 1.0.1 is a source-only release of the reusable synthesis core, with links
to the original native instrument source. The OP-1 integration uses 44.1 kHz
synthesis and four voices. There is no FAST mode in the v1 profile.

Version 1.0.1 corrects Mod noise-envelope bursts. Mod sounds can change audibly;
Exp/Linear envelopes and oscillator reset phase are unchanged. Saved patch
formats are unchanged. Builders must regenerate prepared coefficient data.

[GIN 1.0.1 Source Release](https://github.com/squarewave-studio/op1-mods/releases/tag/gin/v1.0.1)
· [Source And Build Instructions](docs/BUILDERS.md)

Native font rendering, host integration, packaging and hardware testing belong
to the firmware build. This release has no `.knl`, `.op1p` or complete firmware
download. Use the firmware maintainer's instructions for their Gin package.

Very dense patterns can still break up when a lot else on the OP-1 is running
at the same time.

[Controls And Saving](docs/CONTROLS.md) · [Compatibility](docs/COMPATIBILITY.md) · [Release Notes](CHANGELOG.md)

## Credits

GIN is by SquareWave Studio. The synthesis core derives from Eric Lewis's
MIT-licensed [libpo](https://github.com/ericlewis/libpo).
Factory sounds adapt eight of [CNTR RNDM's free PO-32 kits](https://youtube.com/playlist?list=PLlkIAiu0_jPw3DgmZm1RbmFe86rzuWU2o).
The original sounds are CNTR RNDM's; GIN adds the 24-key layout and variations.
op1r is by pcppcp and contributors. Hundekraft contributed the optional
block-renderer optimisation, native resampling improvements and output-level
matching with soft-knee limiting. See [component and licence notes](../../THIRD_PARTY.md).

If GIN has been useful, please consider
[supporting Eric Lewis on GitHub Sponsors](https://github.com/sponsors/ericlewis).

Thanks to [Teenage Engineering](https://teenage.engineering/) for making excellent
hardware and to [Sonic Charge](https://soniccharge.com/microtonic) for the original
Microtonic drum synth. Support Sonic Charge by
[buying Microtonic](https://soniccharge.onfastspring.com/microtonic).

GIN is not affiliated with, authorised by, sponsored by or endorsed by Teenage
Engineering AB or Sonic Charge / NuEdge Development AB. All trademarks remain
their owners' property. [Ownership and source-publication notes](../../THIRD_PARTY.md#independence-and-trademarks).

## Support My Open Source Work

GIN is free. If it's been useful, you can leave a thank-you tip. It doesn't
unlock anything. It just helps me keep building mods like this.

- [Buy me a coffee ($5)](https://buy.squarewave.studio/checkout/buy/880667af-df68-45e1-b8e5-e36c2b5de26e)
- [Buy me lunch ($10)](https://buy.squarewave.studio/checkout/buy/eeb321a3-dea8-486d-9ef7-0794584feb9c)
- [Buy me dinner ($20)](https://buy.squarewave.studio/checkout/buy/edba5162-42c0-4c27-bc95-f41096c2451d)
- [Choose your own amount](https://buy.squarewave.studio/checkout/buy/59446300-c1a9-43d5-90ce-6b4c9f4252c7) (from $3)
