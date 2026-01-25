# case_008: ct_prio seq hold warning on unused

## Reproduction
1. From repo root:
   - `make -C tests/data/openc910/bug_cases/case_008 run`
2. Verilator quick check:
   - `make -C tests/data/openc910/bug_cases/case_008 run_verilator`
3. Wolf emit + Verilator:
   - `make -C tests/data/openc910/bug_cases/case_008 run_wolf_sv_parser_verilator`

## Current behavior
`wolf-sv-parser` emits warnings during elaborate:

```
[elaborate] [WARN] ../../C910_RTL_FACTORY/gen_rtl/ciu/rtl/ct_prio.v:41:1 - seq always missing hold value for false branch (signal = unused)
  statement: always@(posedge clk or negedge rst_b)
```

The `case_008` reproduction command triggers the same warnings (repeated for each generate
instance).

## Inputs (minimized)
- Filelist: `tests/data/openc910/bug_cases/case_008/filelist.f`
  - `tests/data/openc910/C910_RTL_FACTORY/gen_rtl/ciu/rtl/ct_prio.v`
  - `tb_case_008.v`

## Initial analysis
- The sequential always block assigns `{prio, unused}` only on reset, while the non-reset
  branch updates `prio` but never assigns `unused`, triggering the missing-hold warning.

## Fix notes
- Root cause: the sequential LHS converter did not route concatenation LHS that contains
  memory elements through the memory write path. As a result, reset writes to `prio[i]`
  were dropped in the wolf emit flow, causing `sel` to resolve to `2'b11`.
- Fix: added concat-aware handling in the seq LHS converter to slice RHS and dispatch each
  operand through memory write handling (or fallback to normal writes). This restores
  `prio` reset behavior and makes wolf emit match RTL for case_008.
