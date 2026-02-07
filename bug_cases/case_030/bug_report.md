# C910 Bug Case 030: ct_idu_id_decd LSU Instruction Type Decode

## Summary
IDU (Instruction Decode Unit) 模块 `ct_idu_id_decd` 在完整系统仿真中 LSU 指令类型解码失败，导致 "no instructions retired" 错误。

## Problem Description

### 关键发现（差异点）
- **ref**：[c910-id-dp] `inst*_lsu` 在约 cycle=4036 起持续出现，随后 [c910-is-dp] `lsiq_dp_en*` 与 `create0/1_{load,store}` 也开始翻转。
- **wolf**：`inst*_vld` 有翻转（约 cycle=2079 起），但整个日志没有任何 `inst*_lsu`、`lsiq_dp_en*` 或 `create0/1_{load,store}` 记录。
- **wolf** 依旧在 100k 周期报 "no instructions retired"。

### 日志对比
```
# ref log (130k lines)
[c910-id-dp] cycle=4036 inst1_lsu 0->1 inst=0x0000fd86 type=0x030
[c910-id-dp] cycle=4036 inst2_lsu 0->1 inst=0x0000f9a2 type=0x030
[c910-is-dp] cycle=4038 lsiq_dp_en0 0->1
[c910-is-dp] cycle=4038 create0_store 0->1

# wolf log (457 lines)
[c910-id-dp] cycle=2079 inst2_vld 0->1
[c910-id-dp] cycle=2079 inst1_vld 0->1  
[c910-id-dp] cycle=2079 inst0_vld 0->1
# No inst*_lsu signals at all!
```

### 结论 / 可疑模块
- 收敛点已落在 IDU 解码 `inst_type` 这一环节。
- 最小可疑模块是 `ct_idu_id_decd`（或其上游在 `ct_idu_id_dp` 中的 opcode 提取/拼接）。

## Module Under Test
- **DUT**: `ct_idu_id_decd`
- **File**: `tests/data/openc910/C910_RTL_FACTORY/gen_rtl/idu/rtl/ct_idu_id_decd.v`
- **Function**: Instruction decode - generates `x_inst_type[9:0]` and other control signals

### Key Outputs
- `x_inst_type[9:0]`: Instruction type (10-bit one-hot encoding)
  - `LSU`    = 10'b0000010000 (bit 4) - Load/Store Unit
  - `LSU_P5` = 10'b0000110000 (bits 4,5) - LSU Pipe 5 (store instructions)
- `x_dst_vld`: Destination register valid
- `x_src0_vld`, `x_src1_vld`: Source register valid

## Key Observation

### 隔离测试通过
单独的 `ct_idu_id_decd` 模块测试通过了：
- 输入 `x_inst` 直接设置指令编码
- 输出 `x_inst_type` 正确识别 LSU/LSU_P5 类型
- ref 和 wolf 都通过

### 完整系统测试失败
在完整 C910 系统仿真中：
- `inst*_vld` 信号正常（说明指令到达 ID stage）
- 但 `inst*_lsu` 信号完全缺失（说明 `inst_type` 没有正确解码）

这表明问题可能出在：
1. `ct_idu_id_dp` 中 `ct_idu_id_decd` 实例的输入信号异常
2. 控制信号或流水线状态影响了解码
3. 模块间连接问题

## Repro Steps

### 1. Run reference simulation (uses original RTL)
```bash
make -C tests/data/openc910/bug_cases/case_030 run_c910_bug_case_ref
```
Expected: All tests pass

### 2. Run wolf-sv-parser simulation (uses wolf-emitted SV)
```bash
make -C tests/data/openc910/bug_cases/case_030 run_c910_bug_case
```
Expected: All tests pass (isolated module test)

### 3. Full C910 system test
```bash
make run_c910_test  # This reproduces the actual bug
```

## Test Coverage
Test cases include:
- Load instructions: LB, LH, LW, LD (expect LSU type)
- Store instructions: SB, SH, SW, SD (expect LSU_P5 type)
- Compressed loads: C.LW, C.LD (expect LSU type)
- Compressed stores: C.SW, C.SD (expect LSU_P5 type)
- ALU instructions: ADDI, ADD (expect ALU type, for comparison)

## Expected vs Actual
- **Expected**: Load instructions should have `x_inst_type[4]=1` (LSU bit)
- **Expected**: Store instructions should have `x_inst_type[5:4]=2'b11` (LSU_P5 bits)
- **Actual (isolated test)**: Both ref and wolf PASS ✓
- **Actual (full system)**: wolf FAIL - no LSU instructions detected

## Files
- `filelist.f`: RTL file list
- `tb_case_030.cpp`: Testbench with instruction encoding helpers
- `Makefile`: Build and run targets
- `coverage_check.py`: Coverage validation script

## Next Steps for Debugging

### Option 1: Probe ct_idu_id_decd internals
在 `ct_idu_id_decd.v` 内部加最小探针：打印 `x_inst` 与 `x_inst_type`，限定 `ctrl_dp_id_inst*_vld` 时触发。

### Option 2: Compare id_inst*_inst at ct_idu_id_dp output
在 `ct_idu_id_dp.v` 输出处加打印：对比 `id_inst*_inst` 在 ref/wolf 是否一致，确认是不是 opcode 位域被错误拼接。

### Option 3: Create ct_idu_id_dp minimal case
创建包含 `ct_idu_id_dp` 的最小 bugcase，测试其在完整上下文中的行为。

## References
- Log files: `build/logs/c910/c910_ref_coremark_20260207_125107.log`
- Log files: `build/logs/c910/c910_wolf_coremark_20260207_125107.log`
