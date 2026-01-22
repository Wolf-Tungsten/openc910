# case_002: Value already has a defining operation (slice vs blackbox)

## Reproduction
1. From repo root:
   - `make run_c910_test`
2. From this case directory:
   - `make -C tests/data/openc910/bug_cases/case_002 run`

## Current behavior
`wolf-sv-parser` aborts during emit:

```
terminate called after throwing an instance of 'std::runtime_error'
  what():  Value already has a defining operation; value_41 (_rhs_val_range_slice_31_0) w=128 unsigned @ct_l2cache_top.v:161:26; new_def=op_12 kind=kBlackbox (x_ct_l2cache_data_array_bank0) @ct_l2cache_top.v:157:24; existing_def=op_15 kind=kSliceStatic (_rhs_op_range_slice_31_0) @ct_l2cache_top.v:161:26
Aborted (core dumped)
```

## Inputs (minimized)
- Filelist: `tests/data/openc910/bug_cases/case_002/filelist.f`
  - `ct_l2cache_top.v` (extracted from `C910_RTL_FACTORY`)
  - `stub_modules.v` (empty-body stubs to force blackbox instances)
  - `tb_case_002.v`

## Initial analysis
- The conflict points to `l2c_data_dout[127:0]` in `ct_l2cache_top.v` being driven both by a blackbox output
  (`x_ct_l2cache_data_array_bank0`) and a slice op derived from the same net.
- Likely duplicate def occurs when resolving slice values attached to blackbox outputs.

## Fix direction (to validate)
1. Confirm the slice op creation path for part-selects of blackbox outputs and ensure it does not claim the original
   value as a new defining op.
2. Add a guard or create a new value for slice results instead of reusing the blackbox result value.
