# case_034 - ct_iu_alu ALU Module Test

## Module Overview
- **Module**: `ct_iu_alu` - Integer Unit ALU
- **Path**: `tests/data/openc910/C910_RTL_FACTORY/gen_rtl/iu/rtl/ct_iu_alu.v`
- **Description**: The ALU module performs arithmetic, logic, shift, and miscellaneous operations for the C910 processor's integer unit.

## Supported Operations

### Adder Operations
- ADD, ADDW (word), SUB, SUBW (word)
- SLT (set less than)
- MAX, MAXW, MIN, MINW
- ADDSL (add with shift)

### Shifter Operations
- SL (shift left), SR (shift right)
- SLW, SRW (word variants)
- EXT (extract with sign/zero extension)

### Logic Operations
- AND, OR, XOR
- LUI (load upper immediate)
- CLI (compressed load immediate)

### Misc Operations
- MV (conditional move: MVEQZ, MVNEZ)
- TSTNBZ (test none byte zero)
- TST (test bit)
- FF1/FF0 (find first one/zero)
- REV/REVW (byte reverse)

## Repro

1. `make -C tests/data/openc910/bug_cases/case_034 run`
2. `make -C tests/data/openc910/bug_cases/case_034 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_034 run_c910_bug_case`

## Expected

- Reference simulation (RTL directly) should pass all tests with ~60%+ coverage
- Wolf-sv-parser emitted simulation should behave identically to reference

## Actual

- ✅ Reference simulation: All tests passed, 62.4% line coverage (390/625)
- ✅ Wolf-sv-parser simulation: All tests passed, 98.6% line coverage (563/571)

## Status

**WORKING** - wolf-sv-parser successfully processes the `ct_iu_alu` module without errors.

## Minimization notes

- Kept modules: `ct_iu_alu`, `gated_clk_cell`
- Stubs added: None
- Rationale: ALU is self-contained with only clock gating cell as dependency

## Notes

- The wolf-sv-parser achieved higher coverage (98.6%) compared to the original RTL (62.4%) because it simplifies/combines some logic paths during transformation.
- Two warnings are emitted about ignoring timing control in `always @(*)` blocks - these are expected and don't affect functionality.
