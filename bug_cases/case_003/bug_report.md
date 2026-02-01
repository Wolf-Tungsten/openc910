# case_003 - const-fold fails on abbreviated decimal constValue

## Error file
- Path: tests/data/openc910/C910_RTL_FACTORY/gen_rtl/iu/rtl/ct_iu_div_entry.v
- Snippet:

```
[transform] [const-fold] [error] Failed to parse constValue '258'd62771017353866807634955070562867279526...e20': Digit 254 too large for radix 10 (ct_iu_div_entry::)
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_003 run`
2. `make -C tests/data/openc910/bug_cases/case_003 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_003 run_c910_bug_case`

## Expected

- `run_c910_bug_case` completes, emitting SV/JSON and running Verilator without const-fold parse errors.

## Actual

- `run_c910_bug_case` fails in wolf-sv-parser with const-fold parse errors on a 258-bit decimal constValue.

## Minimization notes

- Kept modules: ct_iu_div_entry, gated_clk_cell
- Stubs added: none
- Rationale: ct_iu_div_entry triggers the abbreviated decimal literal when constant expressions are lowered.

## Original log

- build/artifacts/c910_run.log
