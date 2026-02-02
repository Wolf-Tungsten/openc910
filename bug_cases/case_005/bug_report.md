# case_005 - inout port connection handling fails

## Error file
- Path: tests/data/openc910/smart_run/logical/common/soc.v
- Snippet:

```
[convert] [ERROR] ../logical/common/soc.v:49:18 - Skipping instance with unsupported inout connection
  statement: inout   [7  :0]  b_pad_gpio_porta;
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_005 run`
2. `make -C tests/data/openc910/bug_cases/case_005 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_005 run_c910_bug_case`

## Expected

- `run_c910_bug_case` completes, emitting SV/JSON and running Verilator without inout connection errors.

## Actual

- `run_c910_bug_case` fails in wolf-sv-parser with `Skipping instance with unsupported inout connection`.

## Minimization notes

- Kept pattern: inout port on `soc` connected to a top-level inout port in `sim_top`.
- Added internal tri-state drive control in `soc` (`b_pad_gpio_porta__out/__oe`) and sampled the inout in `soc` to cover tri-state behavior.
- TB samples the inout during the tri-state phase (Z collapses to 0 under Verilator).
- Reduced RTL to two modules (`sim_top`, `soc`) to isolate the inout connection.

## Original log

- build/artifacts/c910_run.log
