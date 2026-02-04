# case_016 - Missing XMR port connections on sibling instance

## Error file
- Path: tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv
- Snippet:

```
%Warning-PINMISSING: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv:3848:10: Instance has missing pin: '__xmr_r_576c484e999f5a56'
 3848 |   ct_top x_ct_top_1 (
      |          ^~~~~~~~~~
                     /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv:10548:22: ... Location of port declaration
10548 |   output wire [63:0] __xmr_r_576c484e999f5a56,
      |                      ^~~~~~~~~~~~~~~~~~~~~~~~
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_016 run`
2. `make -C tests/data/openc910/bug_cases/case_016 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_016 run_c910_bug_case`

## Expected

- Wolf-emitted SV adds XMR output ports to `ct_top` **and** updates all `ct_top` instances so Verilator does not warn about missing pins.

## Actual

- Wolf output adds the `__xmr_r_*` port to `ct_top`, connects it only on `x_ct_top_0`, and leaves `x_ct_top_1` missing the pin, triggering `PINMISSING` warnings.

## Minimization notes

- Kept module: `sim_top` (copied from `tests/data/openc910/smart_run/logical/tb/sim_top.v`)
- Stubs added: `stub_modules.v` (minimal hierarchy to satisfy the XMR path)
- Adjustment: `cpu_top` instantiates both `x_ct_top_0` and `x_ct_top_1` so XMR ports are present but only one instance is referenced.

## Original log

- User-provided warning snippet (no log file path)
