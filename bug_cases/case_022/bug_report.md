# case_022 - NOLATCH on ct_rtu_rob_entry create mux

## Error file
- Path: build/artifacts/c910_run.log
- Snippet:

```
%Warning-NOLATCH: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv:587184:3: No latches detected in always_latch block
587184 |   always_latch begin
       |   ^~~~~~~~~~~~
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_022 run`
2. `make -C tests/data/openc910/bug_cases/case_022 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_022 run_c910_bug_case`

## Expected

- Wolf-emitted SV should keep the create-data mux combinational (no always_latch), so Verilator should not emit NOLATCH for the create mux.

## Actual

- Wolf emits `always_latch` for the create-data mux, and Verilator warns: `No latches detected in always_latch block`.

## Minimization notes

- Kept modules: `ct_rtu_rob_entry`, `gated_clk_cell`.
- Stubs added: none.
- Rationale: `ct_rtu_rob_entry` contains the create-data case statement that is lowered into a spurious latch; `gated_clk_cell` is the only instantiation dependency.
