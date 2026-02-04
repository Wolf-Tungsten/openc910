# case_014 - plic_32to1_arb const-fold perf spike

## Error file
- Path: build/artifacts/c910_run.log
- Snippet:

```
[const-fold] [plic_32to1_arb__p4164693972130376995] start
[const-fold] [plic_32to1_arb__p4164693972130376995] seed constants ops=19376, constants=19376, deduped=true, elapsed_ms=815
[const-fold] [plic_32to1_arb__p4164693972130376995] iter 0: ops=47301, folded=11142, new_consts=11142, erased=11142, changed=true, elapsed_ms=44103
[const-fold] [plic_32to1_arb__p4164693972130376995] iter 1: ops=41122, folded=32, new_consts=32, erased=32, changed=true, elapsed_ms=857
[const-fold] [plic_32to1_arb__p4164693972130376995] const-fold done (changed=true, failed=false, warnings=0, warnings_suppressed=0, elapsed_ms=45874)
```

## Repro

1. `make -C tests/data/openc910/bug_cases/case_014 run`
2. `make -C tests/data/openc910/bug_cases/case_014 run_c910_bug_case_ref`
3. `make -C tests/data/openc910/bug_cases/case_014 run_c910_bug_case`

## Expected

- Constant folding for the plic_32to1_arb graph completes in a reasonable time (on the order of seconds, not tens of seconds).

## Actual

- The const-fold pass spent ~45s on the plic_32to1_arb graph (iter 0 ~44s).

## Minimization notes

- Kept modules: plic_32to1_arb.v, plic_granu2_arb.v, plic_granu_arb.v
- Stubs added: none
- Rationale: plic_32to1_arb depends on plic_granu2_arb and plic_granu_arb (which includes prio_sel).
