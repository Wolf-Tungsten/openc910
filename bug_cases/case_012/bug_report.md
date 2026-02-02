# case_012 - Instance output merge overlap on unpacked+packed select

## Error file
- Path: tests/data/openc910/C910_RTL_FACTORY/gen_rtl/plic/rtl/plic_hreg_busif.v
- Snippet:

```
[convert] [ERROR] ../../C910_RTL_FACTORY/gen_rtl/plic/rtl/plic_hreg_busif.v:441:28 - Skipping instance output merge with overlapping slices
  statement: .clk_out              (ie_wr_clk[i][rd_idx]),
Build failed: convert encountered errors
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_012 run`
2. `make -C tests/data/openc910/bug_cases/case_012 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_012 run_c910_bug_case`

## Expected

- wolf-sv-parser should emit SV/JSON for `plic_hreg_busif` without slice-merge errors.

## Actual

- convert fails with "Skipping instance output merge with overlapping slices" at `ie_wr_clk[i][rd_idx]`.

## Minimization notes

- Kept modules:
  - `tests/data/openc910/C910_RTL_FACTORY/gen_rtl/plic/rtl/plic_hreg_busif.v`
  - `tests/data/openc910/C910_RTL_FACTORY/gen_rtl/plic/rtl/csky_apb_1tox_matrix.v`
  - `tests/data/openc910/C910_RTL_FACTORY/gen_rtl/clk/rtl/gated_clk_cell.v`
- Stubs added:
  - `stub_modules.v` with `instance_reg_flog` copied from `plic_kid_busif.v`
- Rationale: Required to preserve port directions and local logic while keeping the filelist minimal.
