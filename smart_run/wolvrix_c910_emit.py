#!/usr/bin/env python3

import os
import sys
import time
from pathlib import Path
import wolvrix


def log(message: str) -> None:
    sys.stderr.write(f"[wolvrix-c910] {message}\n")
    sys.stderr.flush()


script_dir = Path(__file__).resolve().parent
repo_root = (script_dir / "../../../..").resolve()

build_dir = Path(os.environ.get("BUILD_DIR", repo_root / "build"))
output_dir = build_dir / "c910" / "wolvrix_emit"
filelist = script_dir / "logical" / "filelists" / "sim_wolf.fl"

output_dir.mkdir(parents=True, exist_ok=True)

log_level = os.environ.get("WOLF_LOG", "warn")

top_name = "sim_top"
emitted_sv = output_dir / f"{top_name}_wolf.sv"
json_path = output_dir / f"{top_name}_wolf.json"

log(f"input filelist: {filelist}")
log(f"output json: {json_path}")
log(f"output sv: {emitted_sv}")

start = time.perf_counter()
log("read_sv start")
design = wolvrix.read_sv(
    None,
    slang_args=["-f", str(filelist), "--top", top_name],
    log_level=log_level,
)
log(f"read_sv done {int((time.perf_counter() - start) * 1000)}ms")

passes = [
    ("xmr-resolve", []),
    ("multidriven-guard", []),
    ("blackbox-guard", []),
    ("hier-flatten", ["-sym-protect", "hierarchy"]),
    ("const-fold", []),
    ("redundant-elim", []),
    ("memory-init-check", []),
    ("dead-code-elim", []),
    ("stats", []),
]
log("pipeline start")
pipeline_start = time.perf_counter()
for pass_name, args in passes:
    start = time.perf_counter()
    log(f"pass {pass_name} start")
    design.run_pass(pass_name, args=args)
    log(f"pass {pass_name} done {int((time.perf_counter() - start) * 1000)}ms")
log(f"pipeline done {int((time.perf_counter() - pipeline_start) * 1000)}ms")

start = time.perf_counter()
log("write_json start")
design.write_json(str(json_path))
log(f"write_json done {int((time.perf_counter() - start) * 1000)}ms")

start = time.perf_counter()
log("read_json start")
design = wolvrix.read_json(str(json_path))
log(f"read_json done {int((time.perf_counter() - start) * 1000)}ms")

start = time.perf_counter()
log("write_sv start")
design.write_sv(str(emitted_sv))
log(f"write_sv done {int((time.perf_counter() - start) * 1000)}ms")
