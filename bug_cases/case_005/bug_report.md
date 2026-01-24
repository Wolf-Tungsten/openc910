# case_005: uTLB DPLRU refill selection (ct_mmu_dplru)

## Reproduction
1. From repo root:
   - `make -C tests/data/openc910/bug_cases/case_005 run`
2. Verilator quick check:
   - `make -C tests/data/openc910/bug_cases/case_005 run_verilator`
3. Wolf emit + Verilator:
   - `make -C tests/data/openc910/bug_cases/case_005 run_wolf_sv_parser_verilator`

## Current behavior
This is a baseline extraction for `ct_mmu_dplru`. Both RTL and wolf emit are expected to
produce the same refill selection output.

## Inputs (minimized)
- Filelist: `tests/data/openc910/bug_cases/case_005/filelist.f`
  - `tests/data/openc910/C910_RTL_FACTORY/gen_rtl/clk/rtl/gated_clk_cell.v`
  - `tests/data/openc910/C910_RTL_FACTORY/gen_rtl/mmu/rtl/ct_mmu_dplru.v`
  - `tb_case_005.v`

## Initial analysis
- The TB checks that refill selection outputs the lowest invalid entry, and when all
  entries are valid it falls back to the PLRU path (reset state -> entry0).
