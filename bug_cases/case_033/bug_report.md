# case_033 - bht mispred mismatch in pcfifo entry

## Error file
- Path: /tmp/c910_ref_pcfifo_entry0.jsonl and /tmp/c910_wolf_pcfifo_entry0.jsonl
- Snippet:

```
ref  time 493873: bju_pcfifo_ex2_bht_mispred=1 bju_pcfifo_ex2_condbr=1 bht_pred=1 condbr=0
wolf time 493873: bju_pcfifo_ex2_bht_mispred=0 bju_pcfifo_ex2_condbr=1 bht_pred=1 condbr=0
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_033 run`
2. `make -C tests/data/openc910/bug_cases/case_033 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_033 run_c910_bug_case`

## Expected

- `x_rf_read_data`, `x_rt_read_data`, `x_vld`, and `x_flush` match the reference RTL for the same inputs.

## Actual

- In full-system diff runs, `bju_pcfifo_ex2_bht_mispred` differs between ref and wolf at entry0/1 around t=493873, while local `bht_pred` and `condbr` are identical.

## Minimization notes

- Kept modules: ct_iu_bju_pcfifo_entry, gated_clk_cell
- Stubs added: none
- Rationale: entry module isolates the mispred storage path and uses only the gated clock cell.
