# case_009: Write-back slice exceeds target width on part-select assign

## Reproduction
1. From repo root:
   - `make -C tests/data/openc910/bug_cases/case_009 run`
2. Verilator quick check:
   - `make -C tests/data/openc910/bug_cases/case_009 run_verilator`
3. Wolf emit + Verilator:
   - `make -C tests/data/openc910/bug_cases/case_009 run_wolf_sv_parser_verilator`

## Current behavior
`wolf-sv-parser` aborts during elaborate:

```
[elaborate] [ERROR] cpu_sub_system_axi_min.v:6:1 - Write-back slice exceeds target width; slice msb=39 expected at most 31
  statement: assign pad_plic_int_vld[ 39 : 0] = xx_intc_vld[ 39 : 0];
```

The `case_009` reproduction command triggers the same failure.

## Inputs (minimized)
- Filelist: `tests/data/openc910/bug_cases/case_009/filelist.f`
  - `../../C910_RTL_FACTORY/gen_rtl/common/rtl/cpu_sub_system_axi_min.v`
  - `tb_case_009.v`

## Initial analysis
- The design assigns two slices of `pad_plic_int_vld`:
  - `pad_plic_int_vld[39:0]` from `xx_intc_vld[39:0]`
  - `pad_plic_int_vld[144-1:32]` to zero
- `pad_plic_int_vld` is declared as `[144-1:0]`, but the write-back memo treats the target width as 32, so the `39:0` slice overflows.
- `cpu_sub_system_axi_min.v` (under `tests/data/openc910/C910_RTL_FACTORY/gen_rtl/common/rtl/`) is a reduced extraction from `tests/data/openc910/smart_run/logical/common/cpu_sub_system_axi.v` to avoid stubbing the full `rv_integration_platform` dependency tree.
