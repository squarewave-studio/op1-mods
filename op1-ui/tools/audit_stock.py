#!/usr/bin/env python3
"""Summarise an owner's local display resources without exporting TE geometry."""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import re
import xml.etree.ElementTree as ET

def audit(folder):
    records = []
    for path in sorted(folder.glob('*.svg')):
        raw = path.read_bytes()
        if b'<!ENTITY' in raw.upper(): raise ValueError(f'Entity declaration: {path.name}')
        cleaned = re.sub(rb'<!DOCTYPE[^>]*>', b'', raw)
        root = ET.fromstring(cleaned)
        tags = Counter(node.tag.rsplit('}', 1)[-1] for node in root.iter())
        paint = Counter(node.get(key).lower() for node in root.iter() for key in ('fill', 'stroke') if re.fullmatch(r'#[0-9a-fA-F]{6}', node.get(key, '')))
        records.append({'file': path.name, 'sha256': hashlib.sha256(raw).hexdigest(),
                        'bytes': len(raw), 'viewBox': root.get('viewBox'),
                        'elements': dict(sorted(tags.items())), 'paint': dict(sorted(paint.items()))})
    if not records: raise ValueError('No SVG files found; supply the local content/display directory')
    return {'scope': 'Static source resources only; not native screen assembly or hardware rendering', 'resources': records}

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('display_directory', type=Path)
    args = parser.parse_args()
    try: print(json.dumps(audit(args.display_directory), indent=2))
    except (ValueError, ET.ParseError, OSError) as error: parser.exit(1, f'{error}\n')

if __name__ == '__main__': main()
