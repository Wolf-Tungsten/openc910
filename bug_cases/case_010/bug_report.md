# case_010: Reset mux condition does not reference reset signal in uart_receive

## Reproduction
1. From repo root:
   - `make -C tests/data/openc910/bug_cases/case_010 run`
2. Verilator quick check:
   - `make -C tests/data/openc910/bug_cases/case_010 run_verilator` (includes line coverage >= 90%)
3. Wolf emit + Verilator:
   - `make -C tests/data/openc910/bug_cases/case_010 run_wolf_sv_parser_verilator`

## Current behavior
`wolf-sv-parser` fails during elaborate:

```
[elaborate] [ERROR] ../../smart_run/logical/uart/uart_receive.v:124:1 - Reset mux condition does not reference reset signal
  statement: always @(posedge sys_clk or negedge rst_b)
```

## Inputs (minimized)
- Filelist: `tests/data/openc910/bug_cases/case_010/filelist.f`
  - `tests/data/openc910/smart_run/logical/uart/uart_receive.v`
  - `tb_case_010.v`

## Initial analysis
- The failing block is the async-reset synchronizer for `sync1`/`sync2` at line 124.
- The reset condition is `if (rst_b == 1'b0)`, which explicitly references the reset,
  so the reset-mux check appears to be mis-identifying the reset condition form.
