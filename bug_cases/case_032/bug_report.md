# case_032 - fpga_ram read latency mismatch

## Error file
- Path: docs/c910/c910_diff_report.md
- Snippet:

```
- Wolf’s fpga_ram model introduces an extra cycle of read latency: it updates __mem_data_0 on the clock edge and then assigns PortADataOut <= __mem_data_0.
- The original fpga_ram.v outputs mem[PortAAddr] on the same clock edge when not writing.
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_032 run`
2. `make -C tests/data/openc910/bug_cases/case_032 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_032 run_c910_bug_case`

## Expected

- When `PortAWriteEnable` is 0, `PortADataOut` reflects `mem[PortAAddr]` on the same rising edge.
- When `PortAWriteEnable` is 1, `PortADataOut` is write-through (`PortADataIn`).

## Actual

- Wolf emit introduces a one-cycle delay on reads (`PortADataOut` lags the addressed memory by one clock).

## Minimization notes

- Kept modules: `fpga_ram` only.
- Stubs added: none.
- Rationale: the mismatch is isolated to the RAM read path semantics in `fpga_ram`.
