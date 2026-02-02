# case_006 - memory read index missing in convert

## Error file
- Path: tests/data/openc910/smart_run/logical/axi/axi2ahb.v
- Snippet:

```
[convert] [ERROR] ../logical/axi/axi2ahb.v:627:7 - Memory read missing address indices
  statement: db_out_reg[127:0] <=   entry_data[rptr[2:0]];
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_006 run`
2. `make -C tests/data/openc910/bug_cases/case_006 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_006 run_c910_bug_case`

## Expected

- `run_c910_bug_case` completes, emitting SV/JSON and running Verilator without memory read index errors.

## Actual

- `run_c910_bug_case` fails in wolf-sv-parser with `Memory read missing address indices`.

## Minimization notes

- Isolated to `axi2ahb` module and its internal unpacked array access.
- Filelist includes only the original RTL path under `tests/data/openc910/smart_run`.
- TB drives deterministic AXI/AHB handshakes and adds coverage stimulus without modifying the RTL.

## Original log

- build/artifacts/c910_run.log
