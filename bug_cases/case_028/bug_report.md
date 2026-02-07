# case_028 - coremark stall (f_spsram_large focus)

## Error file
- Path: build/logs/c910/c910_coremark_20260207_013632.log
- Snippet:

```
*************************************************************
* Error: There is no instructions retired in the last 50000 cycles! *
*              Simulation Fail and Finished!                *
*************************************************************
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_028 run`
2. `make -C tests/data/openc910/bug_cases/case_028 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_028 run_c910_bug_case`

## Expected

- Ref run completes with coverage near 90%.
- Wolf run matches ref behavior for f_spsram_large reads/writes.

## Actual

- `run_c910_bug_case_ref` passed with 100% line coverage (139/139).
- `run_c910_bug_case` passed with 99.17% line coverage (119/120); no functional mismatch observed.

## Minimization notes

- Kept modules: f_spsram_large, ram
- Stubs added: none
- Rationale: f_spsram_large instantiates ram; both needed for Verilator and wolf emit.
