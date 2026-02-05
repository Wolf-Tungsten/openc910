# case_018 - Width mismatch in lsiq entry emit

## Error file
- Path: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv
- Snippet:

```
%Warning-WIDTHEXPAND: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv:613591:34: Operator AND expects 11 bits on the LHS, but LHS's VARREF '__expr_399' generates 1 bits.
                                                                                                                      : ... note: In instance 'sim_top.x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top.x_ct_top_1.x_ct_core.x_ct_idu_top.x_ct_idu_is_lsiq.x_ct_idu_is_lsiq_entry11'
613591 |   assign __expr_402 = __expr_399 & __expr_401;
       |                                  ^
%Warning-WIDTHTRUNC: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv:613591:21: Operator ASSIGNW expects 1 bits on the Assign RHS, but Assign RHS's AND generates 11 bits.
                                                                                                                     : ... note: In instance 'sim_top.x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top.x_ct_top_1.x_ct_core.x_ct_idu_top.x_ct_idu_is_lsiq.x_ct_idu_is_lsiq_entry11'
613591 |   assign __expr_402 = __expr_399 & __expr_401;
       |                     ^
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_018 run`
2. `make -C tests/data/openc910/bug_cases/case_018 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_018 run_c910_bug_case`

## Expected

- Wolf-emitted SV matches reference operand widths (no WIDTHEXPAND/WIDTHTRUNC from Verilator for ct_idu_is_lsiq_entry).

## Actual

- Verilator reports width expand/trunc warnings on an AND used to compute a 1-bit result inside ct_idu_is_lsiq_entry.

## Minimization notes

- Kept modules: ct_idu_is_lsiq_entry, ct_idu_dep_reg_entry, ct_idu_dep_vreg_entry, gated_clk_cell
- Stubs added: none
- Rationale: ct_idu_is_lsiq_entry instantiates dep_reg/vreg entries and gated clock cell; retaining these avoids changing logic that drives the width-sensitive expression.
