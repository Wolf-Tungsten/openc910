# case_020 - inout vector oe emitted as scalar condition

## Error file
- Path: tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv
- Snippet:

```
%Warning-WIDTHTRUNC: ... Logical operator COND expects 1 bit on the Conditional Test, but Conditional Test's VARREF 'b_pad_gpio_porta__oe' generates 8 bits.
assign b_pad_gpio_porta = b_pad_gpio_porta__oe ? b_pad_gpio_porta__out : {8{1'bz}};
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_020 run`
2. `make -C tests/data/openc910/bug_cases/case_020 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_020 run_c910_bug_case`

## Expected

- Emitted SV uses per-bit output-enable for inout buses and Verilator emits no WIDTHTRUNC warning.

## Actual

- Emitted SV uses a vector oe as a scalar ternary condition, triggering WIDTHTRUNC.

## Minimization notes

- Kept modules: inout_vec_oe
- Stubs added: none
- Rationale: isolate a vector inout port driven by per-bit tri-state assigns.
