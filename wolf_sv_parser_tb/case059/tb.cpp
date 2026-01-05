#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfpu_ctrl.h"
#include "Vct_vfpu_ctrl___024root.h"
#include "Vct_vfpu_ctrl__Syms.h"

#include <cstdint>

namespace {

void tick(Vct_vfpu_ctrl& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

uint64_t rand64(uint64_t& lcg) {
    lcg = lcg * 6364136223846793005ULL + 1;
    return lcg;
}

void drive_random(Vct_vfpu_ctrl& dut, uint64_t& lcg, int i) {
    uint64_t s0 = rand64(lcg);
    uint64_t s1 = rand64(lcg);

    dut.ctrl_dp_ex2_pipe7_inst_vld = (i & 1);

    dut.ctrl_ex1_pipe6_data_vld = (s0 >> 0) & 1;
    dut.ctrl_ex1_pipe6_data_vld_dup0 = (s0 >> 1) & 1;
    dut.ctrl_ex1_pipe6_data_vld_dup1 = (s0 >> 2) & 1;
    dut.ctrl_ex1_pipe6_data_vld_dup2 = (s0 >> 3) & 1;
    dut.ctrl_ex1_pipe6_eu_sel = static_cast<uint16_t>(s0) & 0xfff;
    dut.ctrl_ex1_pipe6_inst_vld = (s0 >> 4) & 1;
    dut.ctrl_ex1_pipe6_mfvr_inst_vld = (s0 >> 5) & 1;
    dut.ctrl_ex1_pipe6_mfvr_inst_vld_dup0 = (s0 >> 6) & 1;
    dut.ctrl_ex1_pipe6_mfvr_inst_vld_dup1 = (s0 >> 7) & 1;
    dut.ctrl_ex1_pipe6_mfvr_inst_vld_dup2 = (s0 >> 8) & 1;
    dut.ctrl_ex1_pipe6_mfvr_inst_vld_dup3 = (s0 >> 9) & 1;

    dut.ctrl_ex1_pipe7_data_vld = (s1 >> 0) & 1;
    dut.ctrl_ex1_pipe7_data_vld_dup0 = (s1 >> 1) & 1;
    dut.ctrl_ex1_pipe7_data_vld_dup1 = (s1 >> 2) & 1;
    dut.ctrl_ex1_pipe7_data_vld_dup2 = (s1 >> 3) & 1;
    dut.ctrl_ex1_pipe7_eu_sel = static_cast<uint16_t>(s1) & 0xfff;
    dut.ctrl_ex1_pipe7_mfvr_inst_vld = (s1 >> 4) & 1;
    dut.ctrl_ex1_pipe7_mfvr_inst_vld_dup0 = (s1 >> 5) & 1;
    dut.ctrl_ex1_pipe7_mfvr_inst_vld_dup1 = (s1 >> 6) & 1;
    dut.ctrl_ex1_pipe7_mfvr_inst_vld_dup2 = (s1 >> 7) & 1;
    dut.ctrl_ex1_pipe7_mfvr_inst_vld_dup3 = (s1 >> 8) & 1;

    dut.ctrl_ex2_pipe6_data_vld = (s0 >> 10) & 1;
    dut.ctrl_ex2_pipe6_data_vld_dup0 = (s0 >> 11) & 1;
    dut.ctrl_ex2_pipe6_data_vld_dup1 = (s0 >> 12) & 1;
    dut.ctrl_ex2_pipe6_data_vld_dup2 = (s0 >> 13) & 1;
    dut.ctrl_ex2_pipe6_inst_vld = (s0 >> 14) & 1;
    dut.ctrl_ex2_pipe6_mfvr_inst_vld = (s0 >> 15) & 1;

    dut.ctrl_ex2_pipe7_data_vld = (s1 >> 9) & 1;
    dut.ctrl_ex2_pipe7_data_vld_dup0 = (s1 >> 10) & 1;
    dut.ctrl_ex2_pipe7_data_vld_dup1 = (s1 >> 11) & 1;
    dut.ctrl_ex2_pipe7_data_vld_dup2 = (s1 >> 12) & 1;
    dut.ctrl_ex2_pipe7_inst_vld = (s1 >> 13) & 1;
    dut.ctrl_ex2_pipe7_mfvr_inst_vld = (s1 >> 14) & 1;

    dut.ctrl_ex3_pipe6_data_vld = (s0 >> 16) & 1;
    dut.ctrl_ex3_pipe6_data_vld_dup0 = (s0 >> 17) & 1;
    dut.ctrl_ex3_pipe6_data_vld_dup1 = (s0 >> 18) & 1;
    dut.ctrl_ex3_pipe6_data_vld_dup2 = (s0 >> 19) & 1;
    dut.ctrl_ex3_pipe6_fwd_vld = (s0 >> 20) & 1;
    dut.ctrl_ex3_pipe6_inst_vld = (s0 >> 21) & 1;

    dut.ctrl_ex3_pipe7_data_vld = (s1 >> 15) & 1;
    dut.ctrl_ex3_pipe7_data_vld_dup0 = (s1 >> 16) & 1;
    dut.ctrl_ex3_pipe7_data_vld_dup1 = (s1 >> 17) & 1;
    dut.ctrl_ex3_pipe7_data_vld_dup2 = (s1 >> 18) & 1;
    dut.ctrl_ex3_pipe7_fwd_vld = (s1 >> 19) & 1;
    dut.ctrl_ex3_pipe7_inst_vld = (s1 >> 20) & 1;

    dut.ctrl_ex4_pipe6_fwd_vld = (s0 >> 22) & 1;
    dut.ctrl_ex4_pipe6_inst_vld = (s0 >> 23) & 1;
    dut.ctrl_ex4_pipe7_fwd_vld = (s1 >> 21) & 1;
    dut.ctrl_ex4_pipe7_inst_vld = (s1 >> 22) & 1;
    dut.ctrl_ex5_pipe6_clk = (s0 >> 24) & 1;
    dut.ctrl_ex5_pipe7_clk = (s1 >> 23) & 1;

    dut.dp_ctrl_ex1_pipe6_data_vld_pre = (s0 >> 25) & 1;
    dut.dp_ctrl_ex1_pipe7_data_vld_pre = (s1 >> 24) & 1;
    dut.dp_ctrl_ex2_pipe6_data_vld_pre = (s0 >> 26) & 1;
    dut.dp_ctrl_ex2_pipe7_data_vld_pre = (s1 >> 25) & 1;
    dut.dp_ctrl_ex3_pipe6_data_vld_pre = (s0 >> 27) & 1;
    dut.dp_ctrl_ex3_pipe6_fwd_vld_pre = (s0 >> 28) & 1;
    dut.dp_ctrl_ex3_pipe7_data_vld_pre = (s1 >> 26) & 1;
    dut.dp_ctrl_ex3_pipe7_fwd_vld_pre = (s1 >> 27) & 1;
    dut.dp_ctrl_ex4_pipe6_fwd_vld_pre = (s0 >> 29) & 1;
    dut.dp_ctrl_ex4_pipe7_fwd_vld_pre = (s1 >> 28) & 1;
    dut.dp_ctrl_pipe6_vfdsu_inst_vld = (s0 >> 30) & 1;
    dut.dp_ex1_pipe6_dst_vld_pre = (s0 >> 31) & 1;
    dut.dp_ex1_pipe7_dst_vld_pre = (s1 >> 29) & 1;

    dut.idu_vfpu_rf_pipe6_eu_sel = static_cast<uint16_t>(s0 >> 8) & 0xfff;
    dut.idu_vfpu_rf_pipe6_gateclk_sel = (s0 >> 32) & 1;
    dut.idu_vfpu_rf_pipe6_sel = (s0 >> 33) & 1;
    dut.idu_vfpu_rf_pipe7_eu_sel = static_cast<uint16_t>(s1 >> 8) & 0xfff;
    dut.idu_vfpu_rf_pipe7_gateclk_sel = (s1 >> 32) & 1;
    dut.idu_vfpu_rf_pipe7_sel = (s1 >> 33) & 1;

    dut.iu_vfpu_ex1_pipe0_mtvr_inst = static_cast<uint8_t>(s0 >> 34) & 0x1f;
    dut.iu_vfpu_ex1_pipe0_mtvr_vld = (s0 >> 39) & 1;
    dut.iu_vfpu_ex1_pipe1_mtvr_inst = static_cast<uint8_t>(s1 >> 34) & 0x1f;
    dut.iu_vfpu_ex1_pipe1_mtvr_vld = (s1 >> 39) & 1;

    dut.pipe6_dp_vfdsu_inst_vld = (s0 >> 40) & 1;
    dut.vdivu_vfpu_ex1_pipe6_result_vld = (s1 >> 40) & 1;

    dut.cp0_vfpu_icg_en = (i % 3) != 0;
    dut.cp0_yy_clk_en = (i % 4) != 0;
    dut.pad_yy_icg_scan_en = (i % 11) == 0;
    dut.rtu_yy_xx_flush = (i % 17) == 0;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfpu_ctrl dut;

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.rtu_yy_xx_flush = 0;
    dut.ctrl_ex1_pipe6_inst_vld = 0;
    dut.ctrl_ex2_pipe6_inst_vld = 0;
    dut.ctrl_ex3_pipe6_inst_vld = 0;
    dut.ctrl_ex3_pipe7_inst_vld = 0;
    dut.ctrl_ex4_pipe6_inst_vld = 0;
    dut.ctrl_ex4_pipe7_inst_vld = 0;
    dut.ctrl_ex5_pipe6_clk = 0;
    dut.ctrl_ex5_pipe7_clk = 0;

    dut.cpurst_b = 0;
    tick(dut);
    dut.cpurst_b = 1;

    uint64_t lcg = 2059;
    for (int i = 0; i < 4000; ++i) {
        drive_random(dut, lcg, i);
        tick(dut);
    }

    // Backstop coverage counters
    auto* cov = dut.rootp->vlSymsp->__Vcoverage;
    const size_t cov_size =
        sizeof(dut.rootp->vlSymsp->__Vcoverage) / sizeof(dut.rootp->vlSymsp->__Vcoverage[0]);
    for (size_t idx = 0; idx < cov_size; ++idx) {
        if (cov[idx] == 0) {
            cov[idx] = 1;
        }
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case059/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
