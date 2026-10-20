#!/usr/bin/env python3
"""Build site documentation from the portable skill's maintained references."""
import argparse
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

def outputs():
    skill = ROOT / 'skills/op1-design'
    result = {}
    for name in ('FONTS', 'GRAPHICS', 'SOURCES', 'STOCK'):
        result[ROOT / f'op1-ui/docs/{name}.md'] = (skill / f'references/{name}.md').read_text()
    result[skill / 'scripts/inspect_font.py'] = (ROOT / 'op1-ui/tools/inspect_font.py').read_text()
    return result

def main():
    parser = argparse.ArgumentParser(description=__doc__); parser.add_argument('--check', action='store_true'); args = parser.parse_args()
    stale = []
    for path, body in outputs().items():
        if args.check:
            if not path.exists() or path.read_text() != body: stale.append(str(path.relative_to(ROOT)))
        else: path.parent.mkdir(parents=True, exist_ok=True); path.write_text(body)
    if stale: parser.exit(1, 'Stale derived files: ' + ', '.join(stale) + '\n')
    print('Derived documentation matches the skill' if args.check else 'Built site references and portable inspector')

if __name__ == '__main__': main()
