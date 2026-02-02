# case_013 - unary minus in ct_vfmau_mult1

## Error file
- Path: ../../C910_RTL_FACTORY/gen_rtl/vfmau/rtl/ct_vfmau_mult1.v
- Log: build/artifacts/c910_run.log
- Snippet:

```
[convert] [ERROR] ../../C910_RTL_FACTORY/gen_rtl/vfmau/rtl/ct_vfmau_mult1.v:1267:78 - Unsupported unary operator
  statement: || ($signed(mult1_ex2_expnt_diff[12:0]) > -2);
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_013 run`
2. `make -C tests/data/openc910/bug_cases/case_013 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_013 run_c910_bug_case`

## Expected

- wolf-sv-parser should lower unary plus/minus expressions (e.g. `-2`) without error.

## Actual

- convert fails with `Unsupported unary operator` when parsing `> -2` in `ct_vfmau_mult1.v`.

## Minimization notes

- Kept modules: ct_vfmau_mult1, ct_vfmau_mult_compressor, ct_vfmau_lza, ct_vfmau_lza_42,
  ct_vfmau_lza_32, ct_vfmau_mult_simd_half, ct_vfmau_ff1_10bit, ct_vfmau_lza_simd_half,
  booth_code_v1, compressor_42, compressor_32, gated_clk_cell.
- Stubs added: none.
- Rationale: dependencies required by ct_vfmau_mult1 and its submodules.
