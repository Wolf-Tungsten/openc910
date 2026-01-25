import os
import sys


def main() -> int:
    if len(sys.argv) < 3:
        print("usage: coverage_check.py <coverage.info> <min_pct>")
        return 1

    info_path = sys.argv[1]
    try:
        min_pct = float(sys.argv[2])
    except ValueError:
        print("invalid min_pct:", sys.argv[2])
        return 1

    if not os.path.exists(info_path):
        print("[COVERAGE] missing coverage info:", info_path)
        return 1

    lf = 0
    lh = 0
    da_total = 0
    da_hit = 0
    with open(info_path, "r") as handle:
        for line in handle:
            if line.startswith("LF:"):
                lf += int(line[3:])
            elif line.startswith("LH:"):
                lh += int(line[3:])
            elif line.startswith("DA:"):
                da_total += 1
                try:
                    count = int(line.strip().split(",")[1])
                except (IndexError, ValueError):
                    count = 0
                if count > 0:
                    da_hit += 1

    if lf == 0 and da_total == 0:
        print("[COVERAGE] no line coverage data found")
        return 1

    if lf == 0:
        lf = da_total
        lh = da_hit

    pct = (float(lh) / float(lf)) * 100.0
    print("[COVERAGE] line coverage %.2f%% (%d/%d)" % (pct, lh, lf))
    if pct < min_pct:
        print("[COVERAGE] below threshold %.2f%%" % min_pct)
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
