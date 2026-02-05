# case_023 - UNOPTFLAT circular comb loop in ct_ifu_bht emit

## Error file
- Path: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv
- Snippet:

```
%Warning-UNOPTFLAT: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv:237160:13: Signal unoptimizable: Circular combinational logic: 'sim_top.x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top.x_ct_top_0.x_ct_core.x_ct_ifu_top.x_ct_ifu_bht.bht_pred_array_index'
237160 |   reg [9:0] bht_pred_array_index;
       |             ^~~~~~~~~~~~~~~~~~~~
                    ... Example path: sim_top.x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top.x_ct_top_0.x_ct_core.x_ct_ifu_top.x_ct_ifu_bht.bht_pred_array_index
                    ... Example path: ASSIGNW
                    ... Example path: sim_top.x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top.x_ct_top_0.x_ct_core.x_ct_ifu_top.x_ct_ifu_bht.__expr_155
                    ... Example path: ALWAYS
                    ... Example path: sim_top.x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top.x_ct_top_0.x_ct_core.x_ct_ifu_top.x_ct_ifu_bht.bht_pred_array_index
%Warning-UNOPTFLAT: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv:237162:13: Signal unoptimizable: Circular combinational logic: 'sim_top.x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.x_cpu_top.x_ct_top_0.x_ct_core.x_ct_ifu_top.x_ct_ifu_bht.bht_pred_array_rd_index'
237162 |   reg [9:0] bht_pred_array_rd_index;
       |             ^~~~~~~~~~~~~~~~~~~~~~~
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_023 run`
2. `make -C tests/data/openc910/bug_cases/case_023 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_023 run_c910_bug_case`

## Expected

- Verilator should not report UNOPTFLAT circular combinational logic warnings for the emitted `wolf_emit.sv` when running the minimized ct_ifu_bht case.

## Actual

- Verilator reports UNOPTFLAT circular combinational logic warnings pointing at `bht_pred_array_index` and `bht_pred_array_rd_index` in the emitted `wolf_emit.sv`.

## Minimization notes

- Kept modules: ct_ifu_bht, ct_ifu_bht_pre_array, ct_ifu_bht_sel_array, gated_clk_cell, ct_spsram_1024x64, ct_spsram_128x16, ct_f_spsram_1024x64, ct_f_spsram_128x16, fpga_ram
- Stubs added: none
- Rationale: ct_ifu_bht instantiates the pre/sel arrays and gated clock/memory helpers that define the reported signals.
