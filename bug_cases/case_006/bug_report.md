# case_006: Sequential write target lacks register metadata (ct_had_pcfifo)

## Reproduction
1. From repo root:
   - `make -C tests/data/openc910/bug_cases/case_006 run`
2. Verilator quick check:
   - `make -C tests/data/openc910/bug_cases/case_006 run_verilator`
3. Wolf emit + Verilator:
   - `make -C tests/data/openc910/bug_cases/case_006 run_wolf_sv_parser_verilator`

## Current behavior
`wolf-sv-parser` aborts during elaborate:

```
[elaborate] [NYI] ../../C910_RTL_FACTORY/gen_rtl/had/rtl/ct_had_pcfifo.v:185:1 - Sequential write target lacks register metadata
  statement: always @(posedge cpuclk or negedge cpurst_b)
```

The `case_006` reproduction command triggers the same failure.

## Inputs (minimized)
- Filelist: `tests/data/openc910/bug_cases/case_006/filelist.f`
  - `tests/data/openc910/C910_RTL_FACTORY/gen_rtl/cpu/rtl/cpu_cfig.h` (for `PA_WIDTH`)
  - `tests/data/openc910/C910_RTL_FACTORY/gen_rtl/had/rtl/ct_had_pcfifo.v`
  - `tb_case_006.v`

## Initial analysis
- The failing statement is in the `PCFIFO_GEN` generate loop that writes to `pcfifo_reg[i][WIDTH-1:0]`.
- `pcfifo_reg` is a reg array, and the write targets a packed slice of the memory element.
