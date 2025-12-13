# Agent SOP: Adding a new wolf_sv_parser_tb case

当人类要求创建 caseNNN 时，即参考本文档操作，在 wolf_sv_parser_tb 目录下创建新的 tb

**Case numbering maps to the plan**  
Use `caseNNN` where `NNN` matches the plan entry number (e.g., plan #1 → `case001`, plan #37 → `case037`). Keep this 1:1 mapping to avoid confusion.

This repo uses numbered cases (caseNNN) under `wolf_sv_parser_tb/`. Follow this checklist to add or update a case that runs a Verilator C++ TB with coverage enabled.

## 0) Conventions
- Cases live at `wolf_sv_parser_tb/caseNNN/` (e.g., case001).
- All build artifacts go under `build/caseNNN/`.
- RTL paths in filelists use the `C910_PROJ` variable (default: repo root). Expand filelists with `envsubst`.
- The top-level `Makefile` already handles build/run/coverage; avoid creating per-case makefiles.

## 1) Add filelist
- Create `wolf_sv_parser_tb/caseNNN/filelist.f`.
- Point to RTL using `${C910_PROJ}/C910_RTL_FACTORY/...` (or other project path, but keep the same variable name).
- Keep the list minimal for the DUT under test.

Example:
```
${C910_PROJ}/C910_RTL_FACTORY/gen_rtl/vfalu/rtl/ct_fadd_close_s0_d.v
```

## 2) Add TB
- Create `wolf_sv_parser_tb/caseNNN/tb.cpp`.
- Include Verilator headers and DUT header, drive stimuli, and call `VerilatedCov::write` (via `verilated_cov.h`) with `COV_OUT` or a default path under `build/caseNNN/coverage.dat`.
- 验证通过 return 0，验证不通过返回非 0

Minimal skeleton:
```cpp
#include <verilated.h>
#include <verilated_cov.h>
#include "V<top>.h"
int main(int argc, char** argv) {
  Verilated::commandArgs(argc, argv);
  V<top> dut;
  // drive inputs, call dut.eval() as needed
  VerilatedCov::write("build/caseNNN/coverage.dat");
  return 0;
}
```

## 3) Use the shared Makefile
- The root `Makefile` auto-derives `TOP` from `wolf_sv_parser_tb/plan.md` using the numeric case index → RTL path mapping; just set `CASE`. Only override `TOP` when you intentionally want a different module than the plan entry.
- Variables:
  - `CASE` (e.g., `001`)
  - `C910_PROJ` (optional; defaults to repo root)
- It expands `filelist.f` via `envsubst`, runs Verilator with `--coverage`, builds `sim`, runs it with `COV_OUT` set, then emits `coverage.info`.

Commands:
```
make run CASE=NNN [C910_PROJ=/path/to/proj]
```
Artifacts:
- `build/caseNNN/sim`             : executable
- `build/caseNNN/coverage.dat`    : raw coverage
- `build/caseNNN/coverage.info`   : lcov format
- `build/caseNNN/annot/`          : created by `verilator_coverage --annotate` if you run it manually

## 4) Coverage inspection
- Annotate: `verilator_coverage --annotate build/caseNNN/annot build/caseNNN/coverage.dat`
- Uncovered lines are marked with `%00` in the annotated file.

## 5) Clean
- `make clean` removes the entire `build/` directory.
