# case_015 - Missing clock/reset alias wires in mem_ctrl

## Error file
- Path: tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv
- Snippet:

```
%Error: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv:35835:20: Can't find definition of variable: 'hclk'
 35835 |   always @(posedge hclk or negedge hrst_b) begin
      |                    ^~~~
%Error: /workspace/wolf-sv-parser/tests/data/openc910/smart_run/work/wolf_emit/sim_top_wolf.sv:35835:36: Can't find definition of variable: 'hrst_b'
 35835 |   always @(posedge hclk or negedge hrst_b) begin
      |                                    ^~~~~~
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_015 run`
2. `make -C tests/data/openc910/bug_cases/case_015 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_015 run_c910_bug_case`

## Expected

- Wolf-emitted SystemVerilog declares/assigns internal clock/reset aliases used in sensitivity lists.

## Actual

- Wolf output omits the `hclk`/`hrst_b` alias nets for `mem_ctrl`, producing verilator errors.

## Minimization notes

- Kept modules: mem_ctrl.v, ram.v
- Stubs added: none
- Rationale: mem_ctrl directly instantiates ram and reproduces the missing alias wires.
