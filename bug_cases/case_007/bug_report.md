# case_007 - Unsupported inout slice assignment in gpio

## Error file
- Path: tests/data/openc910/smart_run/logical/gpio/gpio.v:160
- Snippet:

```
[convert] [ERROR] ../logical/gpio/gpio.v:160:8 - Unsupported inout slice assignment
  statement: assign b_pad_gpio_porta[0] = gpio_porta_oe[0] ? gpio_porta_dr[0] : 1'bz;
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_007 run`
2. `make -C tests/data/openc910/bug_cases/case_007 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_007 run_c910_bug_case`

## Expected

- wolf-sv-parser should accept inout slice assignments (tri-state) and emit valid SV.

## Actual

- wolf-sv-parser stops during convert with "Unsupported inout slice assignment".

## Minimization notes

- Kept modules: gpio.v, gpio_apbif.v, gpio_ctrl.v
- Stubs added: none
- Rationale: gpio instantiates gpio_apbif + gpio_ctrl; no other dependencies.
