# case_001: Operation symbol already bound to operation

## Reproduction
1. From repo root:
   - `make run_c910_test`
2. From this case directory:
   - `make -C tests/data/openc910/bug_cases/case_001 run`

## Current behavior
`wolf-sv-parser` aborts during emit:

```
terminate called after throwing an instance of 'std::runtime_error'
  what():  Operation symbol already bound to operation: ifu_icache_tag_wen_hold_0
Aborted (core dumped)
```

The `case_001` reproduction command triggers the same failure.

## Inputs (minimized)
- Filelist: `tests/data/openc910/bug_cases/case_001/filelist.f`
  - `tests/data/openc910/C910_RTL_FACTORY/gen_rtl/ifu/rtl/ct_ifu_icache_if.v`
  - `stub_modules.v` (blackbox stubs for submodules)
  - `tb_case_001.v`

## Initial analysis
- The error comes from `GraphBuilder::bindSymbol`, which rejects duplicate operation symbols.
- The symbol `ifu_icache_tag_wen_hold_0` matches the `_<suffix>_<counter>` naming pattern used by
  comb-always shadow/hold ops, suggesting a name collision when generating hold operations for
  partial assignments to `ifu_icache_tag_wen` in `ct_ifu_icache_if`.

## Fix direction (to validate)
1. Confirm whether multiple AlwaysConverter instances generate identical hold op names inside the
   same Graph (shadowNameCounter_ reset per always block).
2. Make comb hold op naming unique at the Graph level (e.g., include controlInstanceId_ or use
   graph-level name de-duplication).
