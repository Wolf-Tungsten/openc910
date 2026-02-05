# case_019 - WIDTHTRUNC from unary bitwise NOT under reduction

## Error file
- Path: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv
- Snippet:

```
%Warning-WIDTHTRUNC: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv:361189:21: Operator ASSIGNW expects 1 bits on the Assign RHS, but Assign RHS's AND generates 8 bits.
361189 |   assign __expr_811 = __expr_808 & __expr_810;
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_019 run`
2. `make -C tests/data/openc910/bug_cases/case_019 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_019 run_c910_bug_case`

## Expected

- wolf-sv-parser preserves vector widths for unary bitwise NOT and bitwise ops under reduction, so Verilator does not warn about truncation in the emitted SV.

## Actual

- Verilator reports WIDTHTRUNC on wolf-emitted SV due to an intermediate temp being inferred as 1-bit even though the RHS is a vector.

## Minimization notes

- Kept modules: ct_l2c_data, gated_clk_cell.
- Stubs added: stub_modules.v (defines `L2C_DATA_INDEX_WIDTH=13`).
- Rationale: the original warning is from ct_lsu_rb, but ct_l2c_data has a smaller footprint and contains the same reduction pattern (`assign l2c_data_ram_clk_en_x = |(~data_ram_cen[4:0]);`) that triggers the same truncation class in wolf output. The define aligns with the 13-bit index ports used by this module.
