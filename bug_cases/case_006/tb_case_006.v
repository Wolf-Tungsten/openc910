module tb_case_006(
  input         cpuclk,
  input         cpurst_b,
  input         ctrl_pcfifo_ren,
  input         ctrl_pcfifo_wen,
  input         mmu_xx_mmu_en,
  output [63:0] pcfifo_regs_data,
  input         rtu_had_xx_pcfifo_inst0_chgflow,
  input  [38:0] rtu_had_xx_pcfifo_inst0_next_pc,
  input         rtu_had_xx_pcfifo_inst1_chgflow,
  input  [38:0] rtu_had_xx_pcfifo_inst1_next_pc,
  input         rtu_had_xx_pcfifo_inst2_chgflow,
  input  [38:0] rtu_had_xx_pcfifo_inst2_next_pc
);

  ct_had_pcfifo dut (
    .cpuclk(cpuclk),
    .cpurst_b(cpurst_b),
    .ctrl_pcfifo_ren(ctrl_pcfifo_ren),
    .ctrl_pcfifo_wen(ctrl_pcfifo_wen),
    .mmu_xx_mmu_en(mmu_xx_mmu_en),
    .pcfifo_regs_data(pcfifo_regs_data),
    .rtu_had_xx_pcfifo_inst0_chgflow(rtu_had_xx_pcfifo_inst0_chgflow),
    .rtu_had_xx_pcfifo_inst0_next_pc(rtu_had_xx_pcfifo_inst0_next_pc),
    .rtu_had_xx_pcfifo_inst1_chgflow(rtu_had_xx_pcfifo_inst1_chgflow),
    .rtu_had_xx_pcfifo_inst1_next_pc(rtu_had_xx_pcfifo_inst1_next_pc),
    .rtu_had_xx_pcfifo_inst2_chgflow(rtu_had_xx_pcfifo_inst2_chgflow),
    .rtu_had_xx_pcfifo_inst2_next_pc(rtu_had_xx_pcfifo_inst2_next_pc)
  );
endmodule
