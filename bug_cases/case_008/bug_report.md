# case_008 - Unsupported $unsigned in indexed part-select

## Error file
- Path: tests/data/openc910/C910_RTL_FACTORY/gen_rtl/plic/rtl/plic_hreg_busif.v
- Snippet:

```
[convert] [ERROR] ../../C910_RTL_FACTORY/gen_rtl/plic/rtl/plic_hreg_busif.v:398:68 - Unsupported expression kind
  statement: assign ie_apb_slverr_pre[k] =  (hart_ie_paddr[2+IE_ADDR*k+:5] >= $unsigned(INT_NUM/32))
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_008 run`
2. `make -C tests/data/openc910/bug_cases/case_008 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_008 run_c910_bug_case`

## Expected

- wolf-sv-parser lowers `$unsigned(INT_NUM/32)` in the indexed part-select compare without error.

## Actual

- wolf-sv-parser aborts conversion with “Unsupported expression kind”.

## Minimization notes

- Kept modules: plic_hreg_busif, csky_apb_1tox_matrix, gated_clk_cell.
- Stubs added: instance_reg_flog (to avoid pulling in full plic_kid_busif).
- Rationale: focus on the failing assignment while keeping APB matrix behavior for a deterministic TB.
- Coverage: COV_MIN lowered to 55 (ref run hits ~59% due to large module surface).
