# case_001 - pkb_data slice registers conflict

## Error file
- Path: tests/data/openc910/C910_RTL_FACTORY/gen_rtl/ciu/rtl/ct_piu_top.v
- Snippet:

```
Value already has a defining operation; value_180 (pkb_data) w=512 unsigned @../../C910_RTL_FACTORY/gen_rtl/ciu/rtl/ct_piu_top.v:1945:6; new_def=op_1171 kind=kRegister (pkb_data__register_0) @../../C910_RTL_FACTORY/gen_rtl/ciu/rtl/ct_piu_top.v:1953:6; existing_def=op_1151 kind=kRegister (pkb_data__register) @../../C910_RTL_FACTORY/gen_rtl/ciu/rtl/ct_piu_top.v:1945:6
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_001 run`
2. `make -C tests/data/openc910/bug_cases/case_001 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_001 run_c910_bug_case`

## Expected

- `run_c910_bug_case` completes, emitting SV/JSON and running Verilator without throwing a runtime error.

## Actual

- `run_c910_bug_case` throws a runtime error from wolf-sv-parser: multiple defining operations for `pkb_data` when different always blocks assign slices.

## Minimization notes

- Kept modules: ct_piu_top, ct_fifo, ct_prio, gated_clk_cell, cpu_cfig.h
- Stubs added: none
- Rationale: ct_piu_top is the failing module; ct_fifo/ct_prio/gated_clk_cell are the only submodules it instantiates; cpu_cfig.h provides `PA_WIDTH`.

## Original log

- build/artifacts/c910_run.log
