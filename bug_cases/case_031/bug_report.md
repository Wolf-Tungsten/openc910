# case_031 - ct_fifo entry oscillation after create

## Error file
- Path: build/logs/c910/c910_coremark_20260208_091834.log
- Snippet:

```
[TB] fifo_pop_data mismatch: got=0x0 expected=0x20
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_031 run`
2. `make -C tests/data/openc910/bug_cases/case_031 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_031 run_c910_bug_case`

## Expected

- After a create pulse, `fifo_pop_data` remains stable until a pop occurs.

## Actual

- In wolf emit, the entry oscillates after create and `fifo_pop_data` mismatches, causing the TB to fail.

## Minimization notes

- Kept modules: `ct_fifo`, `gated_clk_cell`
- Stubs added: none
- Rationale: `ct_fifo` is the DUT; `gated_clk_cell` is required for its internal clock gating.
