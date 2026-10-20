#!/usr/bin/env python3
"""Build the static design kit. Requires Markdown 3.7; no private asset inputs."""
import argparse
import html
import json
from pathlib import Path
import re
import shutil
import markdown

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'op1-ui'
BASE = 'https://squarewave-studio.github.io/op1-mods/'
REPO = 'https://github.com/squarewave-studio/op1-mods/blob/main/'
DISCLAIMER = 'Unofficial. Not affiliated with, authorised by or endorsed by Teenage Engineering AB.'
FOOTER = f'''<footer><p><a href="https://squarewave.studio/op-patchstudio?utm_source=op1-design-kit&amp;utm_medium=referral&amp;utm_campaign=op1-design-kit">Made by SquareWave Studio. I also make OP-PatchStudio for the OP-1 Field and OP-XY.</a></p><p>{DISCLAIMER}</p><p><a href="licence.html">MIT Licence</a> · <a href="contributing.html">Contributing</a></p></footer>'''
PAGES = {'start.html': ('Start Building', SOURCE / 'README.md'),
         'spec.html': ('Spec', SOURCE / 'docs/DESIGN.md'),
         'fonts.html': ('Stock Font Setup', SOURCE / 'docs/FONTS.md'),
         'graphics.html': ('Graphics And CPU', SOURCE / 'docs/GRAPHICS.md'),
         'components.html': ('Components', SOURCE / 'docs/COMPONENTS.md'),
         'stock.html': ('Stock Screen Patterns', SOURCE / 'docs/STOCK.md'),
         'examples.html': ('Screen Examples', SOURCE / 'docs/EXAMPLES.md'),
         'credits.html': ('Sources And Credits', SOURCE / 'docs/SOURCES.md'),
         'contributing.html': ('Contributing', ROOT / 'CONTRIBUTING.md'),
         'licence.html': ('MIT Licence', ROOT / 'LICENSE')}

def render_page(title, body):
    description = f'{title} for the OP-1 Design Kit. Practical guidance for original OP-1 screens.'
    return f'''<!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>{html.escape(title)} · OP-1 Design Kit</title><meta name="description" content="{html.escape(description)}"><meta property="og:title" content="{html.escape(title)} · OP-1 Design Kit"><meta property="og:description" content="{html.escape(description)}"><meta property="og:image" content="{BASE}og.png"><meta property="og:type" content="website"><link rel="stylesheet" href="preview.css"></head><body><header class="masthead"><a href="https://squarewave.studio/?utm_source=op1-design-kit&amp;utm_medium=referral&amp;utm_campaign=op1-design-kit">SquareWave Studio</a><a href="index.html">Design Kit</a><a href="agents.html">For Agents</a></header><main class="document">{body}</main>{FOOTER}</body></html>'''

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, default=ROOT / 'out/pages')
    args = parser.parse_args(); out = args.output.resolve()
    if not out.is_relative_to(ROOT / 'out') or out == ROOT / 'out':
        parser.error('Build output must be a dedicated directory below out/')
    if out.exists(): shutil.rmtree(out)
    out.mkdir(parents=True, exist_ok=True)
    for folder in ('src', 'examples'):
        shutil.copytree(SOURCE / folder, out / folder, dirs_exist_ok=True)
    for name in ('preview.css', 'index.html', 'llms.txt'):
        shutil.copy2(SOURCE / name, out / name)
    shutil.copy2(ROOT / 'design-kit.json', out / 'design-kit.json')
    shutil.copy2(SOURCE / 'og.png', out / 'og.png')
    shutil.copytree(ROOT / 'skills/op1-design', out / 'skills/op1-design', dirs_exist_ok=True)
    page_by_source = {p.resolve(): name for name, (_, p) in PAGES.items()}
    def rewrite(body, source):
        def link(match):
            label, target = match[1], match[2]
            if '://' in target or target.startswith('#'): return match[0]
            path, _, fragment = target.partition('#')
            absolute = (source.parent / path).resolve()
            if absolute == ROOT / 'design-kit.json': replacement = 'design-kit.json'
            elif absolute in page_by_source: replacement = page_by_source[absolute]
            elif absolute == ROOT / 'skills/op1-design/SKILL.md': replacement = 'skills/op1-design/SKILL.md'
            elif absolute.is_relative_to(SOURCE / 'examples'): replacement = 'examples/' + absolute.name
            else:
                try: replacement = REPO + str(absolute.relative_to(ROOT))
                except ValueError: replacement = target
            return f'[{label}]({replacement}' + ('#' + fragment if fragment else '') + ')'
        return re.sub(r'\[([^\]]+)\]\(([^)]+)\)', link, body)
    for filename, (title, source) in PAGES.items():
        body = source.read_text()
        if source.name == 'LICENSE': body = '# MIT Licence\n\n```text\n' + body + '\n```'
        body = rewrite(body, source)
        result = markdown.markdown(body, extensions=['tables', 'fenced_code'])
        if filename == 'examples.html':
            catalogue = json.loads((SOURCE / 'examples/catalogue.json').read_text())
            cards = ''.join(f'<figure><img src="examples/{item["id"]}.svg" width="320" height="160" alt="{html.escape(item["title"])}: original schematic code render"><figcaption><h2>{html.escape(item["title"])}</h2><p>{html.escape(item["lesson"])}</p></figcaption></figure>' for item in catalogue)
            result = '<h1>Screen Examples</h1><p>Original compositions informed by stock #246 screen families. These are schematic code renders, not stock screens or hardware captures. No font loading is needed.</p><p><a href="stock.html">Stock Resources And Design Lessons ↗</a></p><div class="screen-gallery">' + cards + '</div>'
        (out / filename).write_text(render_page(title, result))
    agents = '''<h1>For Agents</h1><p>Install the op1-design skill in your firmware project.</p><pre><code>npx skills add squarewave-studio/op1-mods</code></pre><p>Select op1-design. It includes the screen rules, stock-font setup and SVG guidance.</p><p><a href="skills/op1-design/SKILL.md">Read The Skill</a> · <a href="design-kit.json">design-kit.json</a> · <a href="spec.html">Spec</a></p><p>The skill owns the shared rules. The Spec and JSON are built from the same source.</p>'''
    (out / 'agents.html').write_text(render_page('For Agents', agents))
    # Only the allowlisted kit is copied, never the repository's .git or private inputs.
    forbidden = [p for p in out.rglob('*') if p.name.lower() in {'opfont.svg', 'kerntable.db', 'colors.svg', 'agents.md', 'claude.md'} or p.suffix.lower() in {'.op1', '.op1p', '.knl', '.elf'}]
    if forbidden: raise SystemExit('Forbidden build inputs: ' + ', '.join(str(p.relative_to(out)) for p in forbidden))
    for page in out.glob('*.html'):
        text = page.read_text()
        if DISCLAIMER not in text or 'og:image' not in text or 'name="description"' not in text: raise SystemExit(f'Missing page metadata/footer: {page}')
    print(f'Built {len(list(out.glob("*.html")))} pages in {out}')

if __name__ == '__main__': main()
