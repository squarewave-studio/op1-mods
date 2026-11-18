#!/usr/bin/env python3
"""Sanitised core checks; these do not measure OP-1 timing."""
import os, subprocess
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
os.chdir(ROOT)
OUT = ROOT / "out"
OUT.mkdir(exist_ok=True)
FLAGS = [os.environ.get("CC", "cc"), "-std=c99", "-O1", "-g", "-UNDEBUG",
         "-Wall", "-Wextra", "-Werror", "-Wno-misleading-indentation",
         "-fsanitize=address,undefined", "-fno-sanitize-recover=all",
         "-Icore", "-Ithird_party/libpo32/core/include", "-Ithird_party/libpo32/core/src"]
COMMON = ["core/gin_prepare.c", "core/tonic_voice.c", "third_party/libpo32/core/src/po32_synth.c"]
def check(name, sources, fast, block, capture=False):
    exe = OUT / name
    subprocess.run(FLAGS + [f"-DGIN_FAST_FIXED={fast}", f"-DGIN_BLOCK_STATE={block}"] + sources + ["-lm", "-o", str(exe)], check=True)
    return subprocess.run([str(exe)], check=True, stdout=subprocess.PIPE if capture else None).stdout
for fast in (0, 1):
    for block in (0, 1):
        label = f"f{fast}_b{block}"
        check("functional_" + label, ["core/gin_integer.c", "core/gin_precision.c"] + COMMON + ["tests/functional.c"], fast, block)
        check("modulation_" + label, ["tests/modulation.c"] + COMMON, fast, block)
        check("render_" + label, ["core/gin_integer.c", "core/gin_precision.c"] + COMMON + ["tests/render_digest.c"], fast, block, True)
check("noise_guard", ["tests/noise_guard.c"], 1, 0)
print("PASS both rounding profiles and block settings; mixed-length renders, canaries, release/retrigger, absorbing silence, long sine modulation and guarded-filter equivalence")
