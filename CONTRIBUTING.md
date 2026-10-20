# Contributing

Contributions to the original OP-1 mods and the [OP-1 UI kit](op1-ui/README.md)
are welcome. Open an issue for a proposed convention, a documented device result
or a focused improvement. Send code/documentation changes on a branch and pull
request; describe the problem, the resulting behavior and what was tested.

## Keep the public source reusable

Original contributions are offered under this repository's [MIT licence](LICENSE).
Keep copyright/licence notices with third-party work and explain its provenance.
The licence does not cover TE firmware, fonts, artwork, sound banks or trademarks.
Do not add extracted firmware, TE code, `opfont.svg`, kerning databases, glyph
geometry/atlases, baked stock-font vector exports or private evidence.
Tests should use invented fixtures. Native op1r/Kennel adapters belong in their
appropriately licensed upstream/fork, not pasted into this MIT kit.

Reference the public source rather than copying it. Add durable URLs, commit or
release identity, access date, what the source establishes and its limits to the
[UI source ledger](op1-ui/docs/SOURCES.md) or the relevant mod's notices. Mark a
new design proposal as a proposal. Do not label a community observation as a TE
specification or an emulator/host result as a device test.

## UI changes

Read [Design](op1-ui/docs/DESIGN.md),
[Fonts](op1-ui/docs/FONTS.md) and [Graphics](op1-ui/docs/GRAPHICS.md).
Use Small/Medium/Large and Blue/Green/White/Orange by default. For an exception,
explain how it improves physical-size legibility or playability. Include the
control map, font mode/metric profile, long-value and state checks. Public
screenshots/examples should use original distributable assets; keep a local
stock-font review separate unless publication rights have been reviewed.

Use clear, plain English in contributions. Edit the rules in
`skills/op1-design/SKILL.md` and its references, then run:

```sh
python3 op1-ui/tools/sync_tokens.py
python3 op1-ui/tools/sync_skill.py
node --test op1-ui/tests/*.test.mjs
python3 -m unittest discover -s op1-ui/tests
node op1-ui/tools/export_examples.mjs
python3 op1-ui/tools/check_svg.py op1-ui/examples/*.svg
git diff --check
```

Inspect the preview at 1× and enlarged. Use a local stock font for a stock-fidelity
claim. Report renderer, asset identity, output dimensions and limitations. A
native-performance change needs the relevant native measurements, not just
smaller SVG bytes or fewer host function calls. CI does not require proprietary assets.

## Before committing

Install [gitleaks](https://github.com/gitleaks/gitleaks) and enable the tracked hook:

```sh
git config core.hooksPath .githooks
```

Stage explicit paths, inspect the staged diff and run the appropriate checks.
Keep PRs focused. Maintainers review before merge. Do not post credentials,
device keys, private logs or copyrighted firmware in an issue or PR.
