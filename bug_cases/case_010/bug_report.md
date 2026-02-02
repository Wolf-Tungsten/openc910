# case_010 - Empty input port connection rejected

## Error file
- Path: tests/data/openc910/smart_run/logical/pmu/pmu.v
- Snippet:

```
[convert] [ERROR] ../logical/pmu/pmu.v:56:17 - Skipping instance with missing port connection
  statement: input           corec_pmu_sleep_out;
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_010 run`
2. `make -C tests/data/openc910/bug_cases/case_010 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_010 run_c910_bug_case`

## Expected

- wolf-sv-parser accepts empty named port connections for inputs (e.g. `.corec_pmu_sleep_out()`), matching standard Verilog behavior.

## Actual

- wolf-sv-parser aborts conversion with “Skipping instance with missing port connection”.

## Minimization notes

- Kept behavior: empty named port connection on input.
- Added minimal modules: `pmu_min.v` (declares `corec_pmu_sleep_out` but does not use it) and `case_010_top.v` (instantiates `pmu` with `.corec_pmu_sleep_out()` unconnected).
- Rationale: isolate the empty input connection while keeping coverage near 90% for the ref run.
- Log path: build/artifacts/c910_run.log
