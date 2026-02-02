# case_004 - hierarchical reference in expression fails to resolve

## Error file
- Path: tests/data/openc910/smart_run/logical/tb/sim_top.v
- Snippet:

```
[convert] [ERROR] ../logical/tb/sim_top.v:50:20 - Unknown hierarchical symbol in expression
  statement: assign retire0 = x_soc.x_cpu_sub_system_axi.x_rv_integration_platform.core0_pad_retire0;
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_004 run`
2. `make -C tests/data/openc910/bug_cases/case_004 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_004 run_c910_bug_case`

## Expected

- `run_c910_bug_case` completes, emitting SV/JSON and running Verilator without hierarchical symbol resolution errors.

## Actual

- `run_c910_bug_case` fails in wolf-sv-parser with `Unknown hierarchical symbol in expression` while lowering `sim_top.v`.

## Minimization notes

- Kept module: `sim_top` (copied from `tests/data/openc910/smart_run/logical/tb/sim_top.v` into this case)
- Stubs added: `stub_modules.v` provides minimal hierarchy (`soc` → `cpu_sub_system_axi` → `rv_integration_platform` → ...)
- Rationale: keep the hierarchical reference path intact without pulling the full C910 tree.

## Original log

- build/artifacts/c910_run.log
