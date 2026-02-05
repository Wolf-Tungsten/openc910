# case_017 - Unsized literal in concat from emit

## Error file
- Path: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv:940041
- Snippet:

```
%Warning-WIDTHCONCAT: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv:940041:36: Unsized numbers/parameters not allowed in concatenations.
                                                                                                                      : ... note: In instance 'sim_top.x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top.x_ct_top_1.x_ct_core.x_ct_vfpu_top.x_ct_vfalu_top_pipe7.x_ct_fcnvt_top.x_set0_ct_fcnvt_double_dp.x_ct_fcnvt_itof_sh'
940041 |   assign __expr_872 = {__expr_655, 0};
       |                                    ^
%Warning-WIDTHCONCAT: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv:940041:34: Unsized numbers/parameters not allowed in replications.
                                                                                                                      : ... note: In instance 'sim_top.x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top.x_ct_top_1.x_ct_core.x_ct_vfpu_top.x_ct_vfalu_top_pipe7.x_ct_fcnvt_top.x_set0_ct_fcnvt_double_dp.x_ct_fcnvt_itof_sh'
940041 |   assign __expr_872 = {__expr_655, 0};
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_017 run`
2. `make -C tests/data/openc910/bug_cases/case_017 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_017 run_c910_bug_case`

## Expected

- Emitted SV should not contain unsized literals inside concatenations/replications; Verilator should not warn with WIDTHCONCAT for this module.

## Actual

- Emitted SV includes unsized `0` literals in concatenations/replications, triggering WIDTHCONCAT warnings in Verilator.

## Minimization notes

- Kept modules: ct_fcnvt_itof_sh.v
- Stubs added: none
- Rationale: ct_fcnvt_itof_sh is the leaf instance referenced in the warning; it is standalone and reproduces the concatenation emission issue.
