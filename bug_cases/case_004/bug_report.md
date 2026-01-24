# case_004: Memory assignment must target full row or single bit

## Reproduction
1. From repo root:
   - `make run_c910_test`
2. From this case directory:
   - `make -C tests/data/openc910/bug_cases/case_004 run`
3. Verilator quick check:
   - `make -C tests/data/openc910/bug_cases/case_004 run_verilator`
4. Wolf emit + Verilator:
   - `make -C tests/data/openc910/bug_cases/case_004 run_wolf_sv_parser_verilator`

## Current behavior
`wolf-sv-parser` aborts during elaborate:

```
[elaborate] [NYI] sync_level2level.v:41:3 - Memory assignment must target full row or single bit
  statement: always @ (posedge clk or negedge rst_b)
```

The `case_004` reproduction command triggers the same failure.

## Inputs (minimized)
- Filelist: `tests/data/openc910/bug_cases/case_004/filelist.f`
  - `tests/data/openc910/C910_RTL_FACTORY/gen_rtl/common/rtl/sync_level2level.v`
  - `tb_case_004.v`

## Initial analysis
- The failing lvalue is `sync_ff[0][SIGNAL_WIDTH-1:0]`, a packed range select of a memory element.
- Elaborate currently rejects range selects on memory rows, even when the range covers the full width.
