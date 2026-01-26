# case_011: Assign width mismatch on GPIO pad tri-state

## Reproduction
1. From repo root:
   - `make -C tests/data/openc910/bug_cases/case_011 run`
2. Verilator quick check:
   - `make -C tests/data/openc910/bug_cases/case_011 run_verilator` (includes line coverage >= 90%)
3. Wolf emit + Verilator:
   - `make -C tests/data/openc910/bug_cases/case_011 run_wolf_sv_parser_verilator`

## Current behavior
`wolf-sv-parser` fails during elaborate:

```
[elaborate] [ERROR] ../logical/gpio/gpio.v:160:8 - Assign width mismatch; lhs=1 rhs=8
  statement: assign b_pad_gpio_porta[0] = gpio_porta_oe[0] ? gpio_porta_dr[0] : 1'bz;
```

## Inputs (minimized)
- Filelist: `tests/data/openc910/bug_cases/case_011/filelist.f`
  - `tests/data/openc910/smart_run/logical/gpio/gpio.v`
  - `tests/data/openc910/smart_run/logical/gpio/gpio_apbif.v`
  - `tests/data/openc910/smart_run/logical/gpio/gpio_ctrl.v`
  - `tb_case_011.v`

## Initial analysis
- The ternary expression in `gpio.v` is 1-bit wide, but elaborate reports the RHS as 8 bits.
- Likely a width-propagation issue around the inout `b_pad_gpio_porta` slice or conditional operator typing.
