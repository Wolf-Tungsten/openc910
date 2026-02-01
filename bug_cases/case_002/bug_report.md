# case_002 - ncq register duplicate define

## Error file
- Path: tests/data/openc910/C910_RTL_FACTORY/gen_rtl/ciu/rtl/ct_ciu_ncq.v
- Snippet:

```
Value already has a defining operation; value_324 (i) w=32 signed @../../C910_RTL_FACTORY/gen_rtl/ciu/rtl/ct_ciu_ncq.v:1437:9; new_def=op_1674 kind=kRegister (i__register_0) @../../C910_RTL_FACTORY/gen_rtl/ciu/rtl/ct_ciu_ncq.v:1458:9; existing_def=op_987 kind=kRegister (i__register) @../../C910_RTL_FACTORY/gen_rtl/ciu/rtl/ct_ciu_ncq.v:1437:9
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_002 run`
2. `make -C tests/data/openc910/bug_cases/case_002 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_002 run_c910_bug_case`

## Expected

- `run_c910_bug_case` completes, emitting SV/JSON and running Verilator without throwing a runtime error.

## Actual

- `run_c910_bug_case` throws a runtime error from wolf-sv-parser: multiple defining operations for `i` when different always blocks assign slices.

## Minimization notes

- Kept modules: ct_ciu_ncq, ct_ciu_ncq_gm, ct_fifo, ct_prio, gated_clk_cell
- Stubs added: none
- Rationale: ct_ciu_ncq is the failing module; ct_fifo/ct_prio/gated_clk_cell are the only instantiated submodules; ct_ciu_ncq_gm is referenced by ct_ciu_ncq.

## Original log

- build/artifacts/c910_run.log
