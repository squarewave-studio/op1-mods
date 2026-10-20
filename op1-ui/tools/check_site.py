#!/usr/bin/env python3
"""Check built local links, page metadata and the public asset boundary."""
import argparse
from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import unquote, urlsplit

class Links(HTMLParser):
    def __init__(self): super().__init__(); self.links = []
    def handle_starttag(self, tag, attrs):
        for name, value in attrs:
            if name in {'href', 'src'} and value: self.links.append(value)

def main():
    parser = argparse.ArgumentParser(description=__doc__); parser.add_argument('folder', type=Path, nargs='?', default=Path('out/pages')); args = parser.parse_args()
    root = args.folder.resolve(); errors = []
    for path in root.rglob('*'):
        if path.is_file() and (path.name.lower() in {'opfont.svg', 'kerntable.db', 'colors.svg', 'agents.md', 'claude.md'} or path.name.endswith('.private.inc')):
            errors.append(f'Forbidden asset: {path.relative_to(root)}')
    for path in root.glob('*.html'):
        body = path.read_text(); links = Links(); links.feed(body)
        for link in links.links:
            url = urlsplit(link)
            if url.scheme or url.netloc or not url.path: continue
            target = (path.parent / unquote(url.path)).resolve()
            if not target.is_relative_to(root) or not target.exists(): errors.append(f'{path.name}: missing local link {link}')
        for marker in ('<title>', 'name="description"', 'og:image', 'MIT Licence', 'Contributing', 'Unofficial. Not affiliated with, authorised by or endorsed by Teenage Engineering AB.'):
            if marker not in body: errors.append(f'{path.name}: missing {marker}')
    if errors: parser.exit(1, '\n'.join(errors) + '\n')
    print('Site links, metadata, footer and asset boundary pass')

if __name__ == '__main__': main()
