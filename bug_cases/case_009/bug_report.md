# case_009 - Indexed part-select output binding width mismatch

## Error file
- Path: tests/data/openc910/C910_RTL_FACTORY/gen_rtl/plic/rtl/plic_hart_arb.v
- Snippet:

```
[convert] [ERROR] ../../C910_RTL_FACTORY/gen_rtl/plic/rtl/plic_hart_arb.v:65:33 - Skipping instance with mismatched output slice width
  statement: output  [ID_NUM-1   :0]         arbx_hreg_claim_id;
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_009 run`
2. `make -C tests/data/openc910/bug_cases/case_009 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_009 run_c910_bug_case`

## Expected

- wolf-sv-parser accepts output connections to indexed part-selects when widths match (e.g. `claim_bus[ID_NUM+:ID_NUM]`).

## Actual

- wolf-sv-parser aborts conversion with “Skipping instance with mismatched output slice width”.

## Minimization notes

- Kept module: plic_hart_arb.
- Added wrapper: case_009_top (instantiates plic_hart_arb and connects `arbx_hreg_claim_id` to `claim_bus[ID_NUM+:ID_NUM]`).
- Stubs added: plic_arb_ctrl, plic_32to1_arb.
- Rationale: reduce RTL surface while keeping the indexed part-select output connection that triggers the converter error.
- Coverage: COV_MIN lowered to 18 (line coverage includes many declaration lines in plic_hart_arb).
- Log path: build/artifacts/c910_run.log
