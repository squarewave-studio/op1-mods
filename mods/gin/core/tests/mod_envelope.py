#!/usr/bin/env python3
"""Mod envelope regression checks against measured reference behaviour."""
import math
import os
import struct
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "out"
FLAGS = [os.environ.get("CC", "cc"), "-std=c99", "-O2", "-g", "-UNDEBUG",
         "-ffp-contract=off", "-Wall", "-Wextra", "-Werror",
         "-Wno-misleading-indentation", "-fsanitize=address,undefined",
         "-fno-sanitize-recover=all", "-Icore",
         "-Ithird_party/libpo32/core/include", "-Ithird_party/libpo32/core/src"]
SOURCES = ["core/gin_integer.c", "core/gin_precision.c", "core/gin_prepare.c",
           "core/tonic_voice.c", "third_party/libpo32/core/src/po32_synth.c"]


def f32(x):
    return struct.unpack("f", struct.pack("f", x))[0]


def build(name, sources, fast=1, block=0):
    path = OUT / name
    subprocess.run(FLAGS + [f"-DGIN_FAST_FIXED={fast}", f"-DGIN_BLOCK_STATE={block}"] +
                   sources + ["-lm", "-o", str(path)], cwd=ROOT, check=True)
    return path


def run(path, args):
    return subprocess.run([str(path)] + [str(a) for a in args], check=True,
                          capture_output=True, text=True).stdout.strip().splitlines()


def parse(line):
    head, reference, integer, finite = [s.strip() for s in line.split("|")]
    count, length = map(int, head.split())
    return count, length, list(map(int, reference.split())), list(map(int, integer.split())), finite


def main():
    OUT.mkdir(exist_ok=True)
    attacks = [.001, .112541]
    p = .0005
    while p < .86:
        attacks.append(p)
        p *= 1.035
    thresholds = [.751369]
    for count in range(3, 103):
        p = math.log((count - 1.9714) / .3769) / 5.5842
        word = struct.unpack("I", struct.pack("f", p))[0]
        for step in (-1, 0, 1):
            thresholds.append(struct.unpack("f", struct.pack("I", word + step))[0])
    for fast in (0, 1):
        for block in (0, 1):
            label = f"f{fast}_b{block}"
            schedule = build("mod_schedule_" + label, SOURCES + ["tests/native-mod-schedule.c"], fast, block)
            lines = run(schedule, [format(f32(a), ".9g") for a in attacks])
            assert len(lines) == len(attacks)
            for attack, line in zip(attacks, lines):
                count, length, reference, integer, finite = parse(line)
                want = sorted({(k * length + count - 1) // count for k in range(count)
                               if (k * length + count - 1) // count < length})
                assert reference == integer == want, (label, attack, line, want)
                assert finite == "1", (label, attack, line)
            lines = run(schedule, ["n" + format(f32(p), ".9g") for p in thresholds])
            assert len(lines) == len(thresholds)
            for p, line in zip(thresholds, lines):
                want = max(2, math.floor(.3769 * math.exp(5.5842 * f32(p)) + 1.9714))
                assert int(line) == want, (label, p, line, want)
            assert int(lines[0]) == 27
            for line in run(schedule, ["1e-45", "1e-37", "1e-30", "1e-7"]):
                _, length, reference, integer, finite = parse(line)
                assert length <= 1 and finite == "1", (label, line)
                assert reference == integer == ([0] if length else []), (label, line)
            blocks = build("mod_blocks_" + label, SOURCES + ["tests/mod_blocks.c"], fast, block)
            subprocess.run([str(blocks)], check=True)
    # The float helper measures burst times from rendered noise power at three rates.
    helper = build("mod_env", ["tests/native-mod-env.c", SOURCES[-1]])
    for ms, count in ((10, 2), (31.5, 3), (94, 4)):
        # Invert t = p * .03981071706 * 10^(2.4*p) seconds.
        lo, hi = 0., 1.
        for _ in range(60):
            mid = (lo + hi) / 2
            if mid * .03981071706 * 10 ** (2.4 * mid) < ms * .001:
                lo = mid
            else:
                hi = mid
        attack = (lo + hi) / 2
        params = [0.] * 21
        params[8] = 1.; params[10] = 1.; params[11] = attack
        params[12] = .776; params[13] = 1.; params[17] = .74
        params[9] = math.log(.707 / .1) / math.log(100000)
        params[16] = .5
        for rate in (44100, 48000, 96000):
            times = [float(t) for t in run(helper, [rate] + params)[0].split() if float(t) <= ms + 1]
            assert len(times) == count, (ms, rate, times)
            for k, t in enumerate(times, 1):
                assert abs(t - k * ms / count) <= .5, (ms, rate, times)
    print(f"PASS Mod schedules ({len(attacks)} attacks/profile), 301 thresholds/profile, tiny attacks and multi-frame rendering; four profiles")


if __name__ == "__main__":
    main()
