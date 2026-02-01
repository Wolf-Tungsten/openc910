# case_001 - duplicate defining operation in ct_piu_top

## Error file
- Path: tests/data/openc910/C910_RTL_FACTORY/gen_rtl/ciu/rtl/ct_piu_top.v
- RUN_LOG: build/artifacts/c910_run.log
- Snippet:

```
what():  Value already has a defining operation; value_180 (pkb_data) w=512 unsigned @../../C910_RTL_FACTORY/gen_rtl/ciu/rtl/ct_piu_top.v:1945:6; new_def=op_1171 kind=kRegister (pkb_data__register_0) @../../C910_RTL_FACTORY/gen_rtl/ciu/rtl/ct_piu_top.v:1953:6; existing_def=op_1151 kind=kRegister (pkb_data__register) @../../C910_RTL_FACTORY/gen_rtl/ciu/rtl/ct_piu_top.v:1945:6
Aborted (core dumped)
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_001 run`
2. `make -C tests/data/openc910/bug_cases/case_001 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_001 run_c910_bug_case`

## Expected

- wolf-sv-parser should elaborate ct_piu_top without reporting multiple defining operations for pkb_data.

## Actual

- wolf-sv-parser aborts with `Value already has a defining operation` referencing ct_piu_top.v lines 1945 and 1953.

## Minimization notes

- Kept modules: ct_piu_top, ct_fifo, ct_prio, gated_clk_cell
- Defines: cpu_cfig.h (PA_WIDTH)
- Stubs added: none
- Rationale: ct_piu_top depends on ct_fifo/ct_prio and PA_WIDTH; gated_clk_cell is instantiated in ct_piu_top.
