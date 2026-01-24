# case_003: comb always contains an invalid statement (ct_ifu_ifctrl)

## Reproduction
1. From repo root:
   - `make run_c910_test`
2. From this case directory:
   - `make -C tests/data/openc910/bug_cases/case_003 run`

## Current behavior
`wolf-sv-parser` reports an NYI error during elaboration:

```
[elaborate] [NYI] ../../C910_RTL_FACTORY/gen_rtl/ifu/rtl/ct_ifu_ifctrl.v:906 - comb always contains an invalid statement; a common cause is procedural assignment to a wire (e.g., port not declared logic/reg)
```

The `case_003` reproduction command triggers the same failure.

## Inputs (minimized)
- Filelist: `tests/data/openc910/bug_cases/case_003/filelist.f`
  - `tests/data/openc910/C910_RTL_FACTORY/gen_rtl/ifu/rtl/ct_ifu_ifctrl.v`
  - `stub_modules.v` (blackbox stub for `gated_clk_cell`)
  - `tb_case_003.v`

## Initial analysis
- The error is emitted for a comb always block around `ct_ifu_ifctrl.v:906`.
- Likely cause is a procedural assignment targeting a net (wire) or an implicit net in that block.

## Fix direction (to validate)
1. Inspect the always-comb statement at `ct_ifu_ifctrl.v:906` and confirm whether the LHS signal is declared as a wire.
2. If so, treat that assignment as a continuous assignment or force the LHS to be a variable in the front end.
