# case_027 - axi_slave128 rdata_s0 divergence around cycle 2132

## Error file
- Paths:
  - `build/logs/c910/c910_ref_coremark_20260206_164449.log`
  - `build/logs/c910/c910_wolf_coremark_20260206_164449.log`
- Snippet (ref vs wolf at cycle 2132):

```
REF:
[c910-window] cycle=2132 ... biu_rdata_lo=0x7c01a073004001b7 biu_rdata_hi=0x1b73001a0736199 ... rdata_s0_lo=0x7c01a073004001b7 rdata_s0_hi=0x1b73001a0736199 ...

WOLF:
[c910-window] cycle=2132 ... biu_rdata_lo=0x7c01a073004001b7 biu_rdata_hi=0x1b73001a0736199 ... rdata_s0_lo=0x61a13001a0730080 rdata_s0_hi=0x6381b77c01a073 ...
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_027 run`
2. `make -C tests/data/openc910/bug_cases/case_027 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_027 run_c910_bug_case`

## Expected

- rdata_s0 should match the memory data path (biu_rdata) and align with ref
  at the same cycle/window.

## Actual

- rdata_s0 diverges at cycle 2132 in wolf while biu_rdata matches ref.
  Subsequent windows show continued mismatch and stalled retirement in wolf.

## Minimization notes

- Kept modules: axi_slave128, f_spsram_large, ram
- Stubs added: none
- Rationale: minimal memory + slave path to reproduce rdata_s0 divergence.
