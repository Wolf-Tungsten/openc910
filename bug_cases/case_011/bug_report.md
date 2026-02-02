# case_011 - instance output to unpacked array triggers missing binding

## Error file
- Path: ../../C910_RTL_FACTORY/gen_rtl/plic/rtl/plic_kid_busif.v
- Snippet:

```
[convert] [ERROR] ../../C910_RTL_FACTORY/gen_rtl/plic/rtl/plic_kid_busif.v:531:19 - Skipping instance with missing output binding
  statement: output [DATA-1:0] data_out;
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_011 run`
2. `make -C tests/data/openc910/bug_cases/case_011 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_011 run_c910_bug_case`

## Expected

- wolf-sv-parser should accept instance outputs connected to unpacked array elements and emit SV/JSON without errors.

## Actual

- wolf-sv-parser emits "Skipping instance with missing output binding" and aborts conversion.

## Minimization notes

- Kept modules: `plic_kid_busif.v` (for `instance_reg_flog`).
- Stubs added: `stub_modules.v` defines `c910_inst_reg_mem_top` wrapper.
- Rationale: wrapper isolates instance output connected to an unpacked array element while keeping the original module definition and error location.
