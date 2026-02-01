#!/usr/bin/env python3
import sys


def parse_coverage(info_path):
    lh = 0
    lf = 0
    da_lh = 0
    da_lf = 0
    try:
        with open(info_path, "r", encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if line.startswith("LH:"):
                    lh += int(line[3:])
                elif line.startswith("LF:"):
                    lf += int(line[3:])
                elif line.startswith("DA:"):
                    parts = line[3:].split(",")
                    if len(parts) >= 2:
                        da_lf += 1
                        if int(parts[1]) > 0:
                            da_lh += 1
    except FileNotFoundError:
        print("[COVERAGE] missing coverage info:", info_path)
        return None

    if lh == 0 and lf == 0 and da_lf > 0:
        return da_lh, da_lf
    if lf == 0:
        return None
    return lh, lf


def main():
    if len(sys.argv) != 3:
        print("usage: coverage_check.py <coverage.info> <min_pct>")
        return 2

    info_path = sys.argv[1]
    try:
        min_pct = float(sys.argv[2])
    except ValueError:
        print("[COVERAGE] invalid min_pct:", sys.argv[2])
        return 2

    result = parse_coverage(info_path)
    if result is None:
        print("[COVERAGE] no line coverage data found")
        return 1

    lh, lf = result
    pct = (lh * 100.0) / lf
    print("[COVERAGE] line coverage %.2f%% (%d/%d)" % (pct, lh, lf))
    if pct < min_pct:
        print("[COVERAGE] below threshold %.2f%%" % min_pct)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
