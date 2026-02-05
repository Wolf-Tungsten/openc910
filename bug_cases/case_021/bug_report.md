# case_021 - Unsigned comparison warnings in axi_interconnect128

## Error file
- Path: tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv
- Snippet:

```
%Warning-UNSIGNED: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv:24019:28: Comparison is constant due to unsigned arithmetic
                                                                                                                  : ... note: In instance 'sim_top.x_soc.x_axi_interconnect'
24019 |   assign __expr_0 = awaddr >= 40'h0;
      |                            ^~

%Warning-CMPCONST: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv:24039:29: Comparison is constant due to limited range
                                                                                                                  : ... note: In instance 'sim_top.x_soc.x_axi_interconnect'
24039 |   assign __expr_10 = awaddr <= 40'hffffffffff;
      |                             ^~
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_021 run`
2. `make -C tests/data/openc910/bug_cases/case_021 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_021 run_c910_bug_case`

## Expected

- Emitted SV should not generate always-true unsigned comparisons that trigger Verilator warnings.

## Actual

- Wolf-sv-parser emits intermediate expressions like `awaddr >= 40'h0` and `awaddr <= 40'hffffffffff` which are always true for 40-bit unsigned values.

## Minimization notes

- Kept modules: axi_interconnect128
- Stubs added: none
- Rationale: Isolate the address range comparison logic that generates constant comparisons.
