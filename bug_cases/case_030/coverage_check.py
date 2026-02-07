#!/usr/bin/env python3
import sys

def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <coverage.info> <min_coverage>")
        sys.exit(1)
    
    cov_file = sys.argv[1]
    min_cov = float(sys.argv[2])
    
    with open(cov_file, 'r') as f:
        content = f.read()
    
    # Parse coverage info
    total_lines = 0
    hit_lines = 0
    
    for line in content.split('\n'):
        if line.startswith('LF:'):
            total_lines = int(line.split(':')[1])
        elif line.startswith('LH:'):
            hit_lines = int(line.split(':')[1])
    
    if total_lines == 0:
        print("[COVERAGE] No lines found")
        sys.exit(0)
    
    coverage = (hit_lines / total_lines) * 100
    print(f"[COVERAGE] Line coverage: {coverage:.2f}% ({hit_lines}/{total_lines})")
    
    if coverage < min_cov:
        print(f"[COVERAGE] FAILED: Coverage {coverage:.2f}% below minimum {min_cov}%")
        sys.exit(1)
    else:
        print(f"[COVERAGE] PASSED: Coverage {coverage:.2f}% meets minimum {min_cov}%")
        sys.exit(0)

if __name__ == '__main__':
    main()
