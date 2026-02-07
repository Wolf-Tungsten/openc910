#!/usr/bin/env python3
import sys

def main():
    if len(sys.argv) < 2:
        print("Usage: coverage_check.py <coverage.info> [min_percent]")
        sys.exit(0)
    
    cov_file = sys.argv[1]
    min_pct = float(sys.argv[2]) if len(sys.argv) > 2 else 90.0
    
    try:
        with open(cov_file, 'r') as f:
            content = f.read()
    except FileNotFoundError:
        print(f"[COV] Error: {cov_file} not found")
        sys.exit(1)
    
    total_lines = 0
    covered_lines = 0
    
    for line in content.split('\n'):
        if line.startswith('DA:'):
            total_lines += 1
            parts = line[3:].split(',')
            if len(parts) >= 2 and int(parts[1]) > 0:
                covered_lines += 1
    
    if total_lines == 0:
        print("[COV] Warning: No line coverage data found")
        sys.exit(0)
    
    coverage = (covered_lines / total_lines) * 100
    print(f"[COV] Line coverage: {covered_lines}/{total_lines} = {coverage:.2f}%")
    
    if coverage < min_pct:
        print(f"[COV] FAIL: Coverage {coverage:.2f}% < minimum {min_pct}%")
        sys.exit(1)
    else:
        print(f"[COV] PASS: Coverage {coverage:.2f}% >= minimum {min_pct}%")
        sys.exit(0)

if __name__ == '__main__':
    main()
