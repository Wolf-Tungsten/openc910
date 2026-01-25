# case_007: BUFGCE comb latch warnings (non-blocking in comb always)

## Reproduction
1. From repo root:
   - `make -C tests/data/openc910/bug_cases/case_007 run`
2. Verilator quick check:
   - `make -C tests/data/openc910/bug_cases/case_007 run_verilator`
3. Wolf emit + Verilator:
   - `make -C tests/data/openc910/bug_cases/case_007 run_wolf_sv_parser_verilator`

## Current behavior
`wolf-sv-parser` emits warnings during elaborate:

```
[elaborate] [WARN] ../../C910_RTL_FACTORY/gen_rtl/common/rtl/BUFGCE.v:28:1 - comb always uses non-blocking assignment; treated as blocking
  statement: always @(I or CE)
[elaborate] [WARN] ../../C910_RTL_FACTORY/gen_rtl/common/rtl/BUFGCE.v:28:1 - comb always branch coverage incomplete; latch inferred (signal = clk_en_af_latch)
  statement: always @(I or CE)
[elaborate] [WARN] ../../C910_RTL_FACTORY/gen_rtl/common/rtl/BUFGCE.v:35:1 - comb always uses non-blocking assignment; treated as blocking
  statement: always @ (clk_en_af_latch )
[elaborate] [WARN] ../../C910_RTL_FACTORY/gen_rtl/common/rtl/BUFGCE.v:28:1 - Latch inferred for procedural block (signal=clk_en_af_latch)
  statement: always @(I or CE)
```

The `case_007` reproduction command triggers the same warnings.

## Inputs (minimized)
- Filelist: `tests/data/openc910/bug_cases/case_007/filelist.f`
  - `tests/data/openc910/C910_RTL_FACTORY/gen_rtl/common/rtl/BUFGCE.v`
  - `tb_case_007.v`

## Initial analysis
- `BUFGCE` models a latch-based clock enable, updating `clk_en_af_latch` only when `I` is low.
- The comb `always` blocks use non-blocking assignments, and the intentional latch behavior
  triggers the branch coverage / latch warnings.
