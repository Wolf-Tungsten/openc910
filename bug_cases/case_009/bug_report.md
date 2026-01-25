# case_009: write-back slice exceeds target width on pad_plic_int_vld

## Reproduction
1. From repo root:
   - `make -C tests/data/openc910/bug_cases/case_009 run`
2. Verilator quick check:
   - `make -C tests/data/openc910/bug_cases/case_009 run_verilator`
3. Wolf emit + Verilator:
   - `make -C tests/data/openc910/bug_cases/case_009 run_wolf_sv_parser_verilator`

## Current behavior
`wolf-sv-parser` fails during elaborate:

```
[elaborate] [ERROR] ../logical/common/cpu_sub_system_axi.v:369:8 - Write-back slice exceeds target width; slice msb=39 expected at most 31
```

## Inputs (minimized)
- Filelist: `tests/data/openc910/bug_cases/case_009/filelist.f`
  - `tests/data/openc910/smart_run/logical/common/cpu_sub_system_axi.v`
  - `stub_modules.v` (stubs for `rv_integration_platform` and `wid_for_axi4` to avoid deep dependency trees)
  - `tb_case_009.v`

## Initial analysis
- `cpu_sub_system_axi.v` declares `pad_plic_int_vld` as `wire [144 - 1 : 0]` and assigns `pad_plic_int_vld[39:0]`.
- The elaborator reports the target width as 32 (msb 31), suggesting a width-evaluation bug around the
  `144 - 1` range expression or the write-back slice bounds for the net.
