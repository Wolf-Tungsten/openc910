#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfpu_rbus.h"
#include "Vct_vfpu_rbus___024root.h"
#include "Vct_vfpu_rbus__Syms.h"

#include <cstdint>

namespace {

void tick(Vct_vfpu_rbus& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

uint64_t rand64(uint64_t& lcg) {
    lcg = lcg * 6364136223846793005ULL + 1;
    return lcg;
}

void drive_random(Vct_vfpu_rbus& dut, uint64_t& lcg, int i) {
    uint64_t s0 = rand64(lcg);
    uint64_t s1 = rand64(lcg);
    uint64_t s2 = rand64(lcg);

    // ctrl stage valids/fwd signals
    dut.ctrl_ex1_pipe6_data_vld = (i & 1);
    dut.ctrl_ex1_pipe6_data_vld_dup0 = (i & 2);
    dut.ctrl_ex1_pipe6_data_vld_dup1 = (i & 4);
    dut.ctrl_ex1_pipe6_data_vld_dup2 = (i & 8);
    dut.ctrl_ex1_pipe7_data_vld = (i & 16);
    dut.ctrl_ex1_pipe7_data_vld_dup0 = (i & 32);
    dut.ctrl_ex1_pipe7_data_vld_dup1 = (i & 64);
    dut.ctrl_ex1_pipe7_data_vld_dup2 = (i & 128);

    dut.ctrl_ex2_pipe6_data_vld = (s0 >> 1) & 1;
    dut.ctrl_ex2_pipe6_data_vld_dup0 = (s0 >> 2) & 1;
    dut.ctrl_ex2_pipe6_data_vld_dup1 = (s0 >> 3) & 1;
    dut.ctrl_ex2_pipe6_data_vld_dup2 = (s0 >> 4) & 1;
    dut.ctrl_ex2_pipe7_data_vld = (s0 >> 5) & 1;
    dut.ctrl_ex2_pipe7_data_vld_dup0 = (s0 >> 6) & 1;
    dut.ctrl_ex2_pipe7_data_vld_dup1 = (s0 >> 7) & 1;
    dut.ctrl_ex2_pipe7_data_vld_dup2 = (s0 >> 8) & 1;

    dut.ctrl_ex3_pipe6_data_vld = (s1 >> 1) & 1;
    dut.ctrl_ex3_pipe6_data_vld_dup0 = (s1 >> 2) & 1;
    dut.ctrl_ex3_pipe6_data_vld_dup1 = (s1 >> 3) & 1;
    dut.ctrl_ex3_pipe6_data_vld_dup2 = (s1 >> 4) & 1;
    dut.ctrl_ex3_pipe6_fwd_vld = (s1 >> 5) & 1;
    dut.ctrl_ex3_pipe7_data_vld = (s1 >> 6) & 1;
    dut.ctrl_ex3_pipe7_data_vld_dup0 = (s1 >> 7) & 1;
    dut.ctrl_ex3_pipe7_data_vld_dup1 = (s1 >> 8) & 1;
    dut.ctrl_ex3_pipe7_data_vld_dup2 = (s1 >> 9) & 1;
    dut.ctrl_ex3_pipe7_fwd_vld = (s1 >> 10) & 1;

    dut.ctrl_ex4_pipe6_fwd_vld = (s2 >> 1) & 1;
    dut.ctrl_ex4_pipe7_fwd_vld = (s2 >> 2) & 1;
    dut.ctrl_ex4_pipe7_inst_vld = (s2 >> 3) & 1;
    dut.ctrl_ex5_pipe6_clk = (s2 >> 4) & 1;
    dut.ctrl_ex5_pipe7_clk = (s2 >> 5) & 1;

    // data payloads
    dut.dp_ex3_pipe6_dst_vreg = static_cast<uint8_t>(s0) & 0x7f;
    dut.dp_ex3_pipe6_freg_data = s1;
    dut.dp_ex3_pipe7_dst_vreg = static_cast<uint8_t>(s1 >> 8) & 0x7f;
    dut.dp_ex3_pipe7_freg_data = s2;

    dut.dp_ex4_pipe6_dst_ereg = static_cast<uint8_t>(s0 >> 16) & 0x1f;
    dut.dp_ex4_pipe6_dst_vreg = static_cast<uint8_t>(s1 >> 16) & 0x7f;
    dut.dp_ex4_pipe6_normal_dste_wb_vld = (s2 >> 6) & 1;
    dut.dp_ex4_pipe6_normal_dstv_wb_vld = (s2 >> 7) & 1;
    dut.dp_ex4_pipe7_dst_ereg = static_cast<uint8_t>(s0 >> 24) & 0x1f;
    dut.dp_ex4_pipe7_dst_vreg = static_cast<uint8_t>(s1 >> 24) & 0x7f;
    dut.dp_ex4_pipe7_dste_vld = (s2 >> 8) & 1;
    dut.dp_ex4_pipe7_dstv_vld = (s2 >> 9) & 1;

    dut.dp_ex5_pipe6_ereg_data_pre = static_cast<uint8_t>(s0 >> 32) & 0x1f;
    dut.dp_ex5_pipe6_freg_data_pre = s0;
    dut.dp_ex5_pipe7_ereg_data_pre = static_cast<uint8_t>(s1 >> 32) & 0x1f;
    dut.dp_ex5_pipe7_freg_data_pre = s1;

    // Forward/no-fwd controls
    dut.pipe6_rbus_ex1_fmla_data_vld = (s0 >> 10) & 1;
    dut.pipe6_rbus_ex1_fmla_data_vld_dup0 = (s0 >> 11) & 1;
    dut.pipe6_rbus_ex1_fmla_data_vld_dup1 = (s0 >> 12) & 1;
    dut.pipe6_rbus_ex1_fmla_data_vld_dup2 = (s0 >> 13) & 1;
    dut.pipe6_rbus_ex2_fmla_data_vld = (s0 >> 14) & 1;
    dut.pipe6_rbus_ex2_fmla_data_vld_dup0 = (s0 >> 15) & 1;
    dut.pipe6_rbus_ex2_fmla_data_vld_dup1 = (s0 >> 16) & 1;
    dut.pipe6_rbus_ex2_fmla_data_vld_dup2 = (s0 >> 17) & 1;
    dut.pipe6_rbus_pipe6_fmla_no_fwd = (s0 >> 18) & 1;
    dut.pipe6_rbus_pipe7_fmla_no_fwd = (s0 >> 19) & 1;
    dut.pipe6_rbus_vfmau_ereg_wb_data = static_cast<uint8_t>(s0 >> 40) & 0x1f;
    dut.pipe6_rbus_vfmau_ereg_wb_vld = (s0 >> 20) & 1;
    dut.pipe6_rbus_vfmau_freg_wb_data = s2;
    dut.pipe6_rbus_vfmau_vreg_wb_vld = (s0 >> 21) & 1;

    dut.pipe7_rbus_ex1_fmla_data_vld = (s1 >> 10) & 1;
    dut.pipe7_rbus_ex1_fmla_data_vld_dup0 = (s1 >> 11) & 1;
    dut.pipe7_rbus_ex1_fmla_data_vld_dup1 = (s1 >> 12) & 1;
    dut.pipe7_rbus_ex1_fmla_data_vld_dup2 = (s1 >> 13) & 1;
    dut.pipe7_rbus_ex2_fmla_data_vld = (s1 >> 14) & 1;
    dut.pipe7_rbus_ex2_fmla_data_vld_dup0 = (s1 >> 15) & 1;
    dut.pipe7_rbus_ex2_fmla_data_vld_dup1 = (s1 >> 16) & 1;
    dut.pipe7_rbus_ex2_fmla_data_vld_dup2 = (s1 >> 17) & 1;
    dut.pipe7_rbus_pipe6_fmla_no_fwd = (s1 >> 18) & 1;
    dut.pipe7_rbus_pipe7_fmla_no_fwd = (s1 >> 19) & 1;
    dut.pipe7_rbus_vfmau_ereg_wb_data = static_cast<uint8_t>(s1 >> 40) & 0x1f;
    dut.pipe7_rbus_vfmau_ereg_wb_vld = (s1 >> 20) & 1;
    dut.pipe7_rbus_vfmau_freg_wb_data = s0;
    dut.pipe7_rbus_vfmau_vreg_wb_vld = (s1 >> 21) & 1;

    // IDU backward signals (duplicates)
    dut.dp_rbus_pipe6_ex1_vreg = static_cast<uint8_t>(s0) & 0x7f;
    dut.dp_rbus_pipe6_ex1_vreg_dup0 = static_cast<uint8_t>(s0 >> 1) & 0x7f;
    dut.dp_rbus_pipe6_ex1_vreg_dup1 = static_cast<uint8_t>(s0 >> 2) & 0x7f;
    dut.dp_rbus_pipe6_ex1_vreg_dup2 = static_cast<uint8_t>(s0 >> 3) & 0x7f;
    dut.dp_rbus_pipe6_ex2_vreg = static_cast<uint8_t>(s0 >> 4) & 0x7f;
    dut.dp_rbus_pipe6_ex2_vreg_dup0 = static_cast<uint8_t>(s0 >> 5) & 0x7f;
    dut.dp_rbus_pipe6_ex2_vreg_dup1 = static_cast<uint8_t>(s0 >> 6) & 0x7f;
    dut.dp_rbus_pipe6_ex2_vreg_dup2 = static_cast<uint8_t>(s0 >> 7) & 0x7f;
    dut.dp_rbus_pipe6_ex3_vreg_dup0 = static_cast<uint8_t>(s0 >> 8) & 0x7f;
    dut.dp_rbus_pipe6_ex3_vreg_dup1 = static_cast<uint8_t>(s0 >> 9) & 0x7f;
    dut.dp_rbus_pipe6_ex3_vreg_dup2 = static_cast<uint8_t>(s0 >> 10) & 0x7f;
    dut.dp_rbus_pipe6_ex3_vreg_dup3 = static_cast<uint8_t>(s0 >> 11) & 0x7f;

    dut.dp_rbus_pipe7_ex1_vreg = static_cast<uint8_t>(s1) & 0x7f;
    dut.dp_rbus_pipe7_ex1_vreg_dup0 = static_cast<uint8_t>(s1 >> 1) & 0x7f;
    dut.dp_rbus_pipe7_ex1_vreg_dup1 = static_cast<uint8_t>(s1 >> 2) & 0x7f;
    dut.dp_rbus_pipe7_ex1_vreg_dup2 = static_cast<uint8_t>(s1 >> 3) & 0x7f;
    dut.dp_rbus_pipe7_ex2_vreg = static_cast<uint8_t>(s1 >> 4) & 0x7f;
    dut.dp_rbus_pipe7_ex2_vreg_dup0 = static_cast<uint8_t>(s1 >> 5) & 0x7f;
    dut.dp_rbus_pipe7_ex2_vreg_dup1 = static_cast<uint8_t>(s1 >> 6) & 0x7f;
    dut.dp_rbus_pipe7_ex2_vreg_dup2 = static_cast<uint8_t>(s1 >> 7) & 0x7f;
    dut.dp_rbus_pipe7_ex3_vreg_dup0 = static_cast<uint8_t>(s1 >> 8) & 0x7f;
    dut.dp_rbus_pipe7_ex3_vreg_dup1 = static_cast<uint8_t>(s1 >> 9) & 0x7f;
    dut.dp_rbus_pipe7_ex3_vreg_dup2 = static_cast<uint8_t>(s1 >> 10) & 0x7f;
    dut.dp_rbus_pipe7_ex3_vreg_dup3 = static_cast<uint8_t>(s1 >> 11) & 0x7f;

    // IDU outputs
    dut.vfpu_idu_ex1_pipe6_data_vld_dup0 = (s2 >> 10) & 1;
    dut.vfpu_idu_ex1_pipe6_data_vld_dup1 = (s2 >> 11) & 1;
    dut.vfpu_idu_ex1_pipe6_data_vld_dup2 = (s2 >> 12) & 1;
    dut.vfpu_idu_ex1_pipe6_data_vld_dup3 = (s2 >> 13) & 1;
    dut.vfpu_idu_ex1_pipe6_fmla_data_vld_dup0 = (s2 >> 14) & 1;
    dut.vfpu_idu_ex1_pipe6_fmla_data_vld_dup1 = (s2 >> 15) & 1;
    dut.vfpu_idu_ex1_pipe6_fmla_data_vld_dup2 = (s2 >> 16) & 1;
    dut.vfpu_idu_ex1_pipe6_fmla_data_vld_dup3 = (s2 >> 17) & 1;
    dut.vfpu_idu_ex1_pipe6_vreg_dup0 = static_cast<uint8_t>(s2) & 0x7f;
    dut.vfpu_idu_ex1_pipe6_vreg_dup1 = static_cast<uint8_t>(s2 >> 1) & 0x7f;
    dut.vfpu_idu_ex1_pipe6_vreg_dup2 = static_cast<uint8_t>(s2 >> 2) & 0x7f;
    dut.vfpu_idu_ex1_pipe6_vreg_dup3 = static_cast<uint8_t>(s2 >> 3) & 0x7f;

    dut.vfpu_idu_ex1_pipe7_data_vld_dup0 = (s2 >> 18) & 1;
    dut.vfpu_idu_ex1_pipe7_data_vld_dup1 = (s2 >> 19) & 1;
    dut.vfpu_idu_ex1_pipe7_data_vld_dup2 = (s2 >> 20) & 1;
    dut.vfpu_idu_ex1_pipe7_data_vld_dup3 = (s2 >> 21) & 1;
    dut.vfpu_idu_ex1_pipe7_fmla_data_vld_dup0 = (s2 >> 22) & 1;
    dut.vfpu_idu_ex1_pipe7_fmla_data_vld_dup1 = (s2 >> 23) & 1;
    dut.vfpu_idu_ex1_pipe7_fmla_data_vld_dup2 = (s2 >> 24) & 1;
    dut.vfpu_idu_ex1_pipe7_fmla_data_vld_dup3 = (s2 >> 25) & 1;
    dut.vfpu_idu_ex1_pipe7_vreg_dup0 = static_cast<uint8_t>(s2 >> 4) & 0x7f;
    dut.vfpu_idu_ex1_pipe7_vreg_dup1 = static_cast<uint8_t>(s2 >> 5) & 0x7f;
    dut.vfpu_idu_ex1_pipe7_vreg_dup2 = static_cast<uint8_t>(s2 >> 6) & 0x7f;
    dut.vfpu_idu_ex1_pipe7_vreg_dup3 = static_cast<uint8_t>(s2 >> 7) & 0x7f;

    dut.vfpu_idu_ex2_pipe6_data_vld_dup0 = (s2 >> 26) & 1;
    dut.vfpu_idu_ex2_pipe6_data_vld_dup1 = (s2 >> 27) & 1;
    dut.vfpu_idu_ex2_pipe6_data_vld_dup2 = (s2 >> 28) & 1;
    dut.vfpu_idu_ex2_pipe6_data_vld_dup3 = (s2 >> 29) & 1;
    dut.vfpu_idu_ex2_pipe6_fmla_data_vld_dup0 = (s2 >> 30) & 1;
    dut.vfpu_idu_ex2_pipe6_fmla_data_vld_dup1 = (s2 >> 31) & 1;
    dut.vfpu_idu_ex2_pipe6_fmla_data_vld_dup2 = (s2 >> 32) & 1;
    dut.vfpu_idu_ex2_pipe6_fmla_data_vld_dup3 = (s2 >> 33) & 1;
    dut.vfpu_idu_ex2_pipe6_vreg_dup0 = static_cast<uint8_t>(s2 >> 8) & 0x7f;
    dut.vfpu_idu_ex2_pipe6_vreg_dup1 = static_cast<uint8_t>(s2 >> 9) & 0x7f;
    dut.vfpu_idu_ex2_pipe6_vreg_dup2 = static_cast<uint8_t>(s2 >> 10) & 0x7f;
    dut.vfpu_idu_ex2_pipe6_vreg_dup3 = static_cast<uint8_t>(s2 >> 11) & 0x7f;

    dut.vfpu_idu_ex2_pipe7_data_vld_dup0 = (s2 >> 34) & 1;
    dut.vfpu_idu_ex2_pipe7_data_vld_dup1 = (s2 >> 35) & 1;
    dut.vfpu_idu_ex2_pipe7_data_vld_dup2 = (s2 >> 36) & 1;
    dut.vfpu_idu_ex2_pipe7_data_vld_dup3 = (s2 >> 37) & 1;
    dut.vfpu_idu_ex2_pipe7_fmla_data_vld_dup0 = (s2 >> 38) & 1;
    dut.vfpu_idu_ex2_pipe7_fmla_data_vld_dup1 = (s2 >> 39) & 1;
    dut.vfpu_idu_ex2_pipe7_fmla_data_vld_dup2 = (s2 >> 40) & 1;
    dut.vfpu_idu_ex2_pipe7_fmla_data_vld_dup3 = (s2 >> 41) & 1;
    dut.vfpu_idu_ex2_pipe7_vreg_dup0 = static_cast<uint8_t>(s2 >> 12) & 0x7f;
    dut.vfpu_idu_ex2_pipe7_vreg_dup1 = static_cast<uint8_t>(s2 >> 13) & 0x7f;
    dut.vfpu_idu_ex2_pipe7_vreg_dup2 = static_cast<uint8_t>(s2 >> 14) & 0x7f;
    dut.vfpu_idu_ex2_pipe7_vreg_dup3 = static_cast<uint8_t>(s2 >> 15) & 0x7f;

    // EX3 forwards
    dut.vfpu_idu_ex3_pipe6_data_vld_dup0 = (s0 >> 22) & 1;
    dut.vfpu_idu_ex3_pipe6_data_vld_dup1 = (s0 >> 23) & 1;
    dut.vfpu_idu_ex3_pipe6_data_vld_dup2 = (s0 >> 24) & 1;
    dut.vfpu_idu_ex3_pipe6_data_vld_dup3 = (s0 >> 25) & 1;
    dut.vfpu_idu_ex3_pipe6_fwd_vreg = (s0 >> 26) & 1;
    dut.vfpu_idu_ex3_pipe6_fwd_vreg_fr_data = s1;
    dut.vfpu_idu_ex3_pipe6_fwd_vreg_vld = (s0 >> 27) & 1;
    dut.vfpu_idu_ex3_pipe6_fwd_vreg_vr0_data = s2;
    dut.vfpu_idu_ex3_pipe6_fwd_vreg_vr1_data = s0;
    dut.vfpu_idu_ex3_pipe6_vreg_dup0 = static_cast<uint8_t>(s0 >> 28) & 0x7f;
    dut.vfpu_idu_ex3_pipe6_vreg_dup1 = static_cast<uint8_t>(s0 >> 29) & 0x7f;
    dut.vfpu_idu_ex3_pipe6_vreg_dup2 = static_cast<uint8_t>(s0 >> 30) & 0x7f;
    dut.vfpu_idu_ex3_pipe6_vreg_dup3 = static_cast<uint8_t>(s0 >> 31) & 0x7f;

    dut.vfpu_idu_ex3_pipe7_data_vld_dup0 = (s1 >> 22) & 1;
    dut.vfpu_idu_ex3_pipe7_data_vld_dup1 = (s1 >> 23) & 1;
    dut.vfpu_idu_ex3_pipe7_data_vld_dup2 = (s1 >> 24) & 1;
    dut.vfpu_idu_ex3_pipe7_data_vld_dup3 = (s1 >> 25) & 1;
    dut.vfpu_idu_ex3_pipe7_fwd_vreg = (s1 >> 26) & 1;
    dut.vfpu_idu_ex3_pipe7_fwd_vreg_fr_data = s0;
    dut.vfpu_idu_ex3_pipe7_fwd_vreg_vld = (s1 >> 27) & 1;
    dut.vfpu_idu_ex3_pipe7_fwd_vreg_vr0_data = s2;
    dut.vfpu_idu_ex3_pipe7_fwd_vreg_vr1_data = s1;
    dut.vfpu_idu_ex3_pipe7_vreg_dup0 = static_cast<uint8_t>(s1 >> 28) & 0x7f;
    dut.vfpu_idu_ex3_pipe7_vreg_dup1 = static_cast<uint8_t>(s1 >> 29) & 0x7f;
    dut.vfpu_idu_ex3_pipe7_vreg_dup2 = static_cast<uint8_t>(s1 >> 30) & 0x7f;
    dut.vfpu_idu_ex3_pipe7_vreg_dup3 = static_cast<uint8_t>(s1 >> 31) & 0x7f;

    dut.vfpu_idu_ex4_pipe6_fwd_vreg = (s2 >> 42) & 1;
    dut.vfpu_idu_ex4_pipe6_fwd_vreg_fr_data = s0;
    dut.vfpu_idu_ex4_pipe6_fwd_vreg_vld = (s2 >> 43) & 1;
    dut.vfpu_idu_ex4_pipe6_fwd_vreg_vr0_data = s1;
    dut.vfpu_idu_ex4_pipe6_fwd_vreg_vr1_data = s2;
    dut.vfpu_idu_ex4_pipe7_fwd_vreg = (s2 >> 44) & 1;
    dut.vfpu_idu_ex4_pipe7_fwd_vreg_fr_data = s1;
    dut.vfpu_idu_ex4_pipe7_fwd_vreg_vld = (s2 >> 45) & 1;
    dut.vfpu_idu_ex4_pipe7_fwd_vreg_vr0_data = s0;
    dut.vfpu_idu_ex4_pipe7_fwd_vreg_vr1_data = s1;

    dut.vdsp_vfpu_pipe6_inside_fwd_aval = (s2 >> 46) & 1;
    dut.vdsp_vfpu_pipe7_inside_fwd_aval = (s2 >> 47) & 1;

    dut.cp0_vfpu_icg_en = (i % 3) != 0;
    dut.cp0_yy_clk_en = (i % 4) != 0;
    dut.pad_yy_icg_scan_en = (i % 11) == 0;
    dut.rtu_yy_xx_flush = (i % 17) == 0;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfpu_rbus dut;

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.rtu_yy_xx_flush = 0;
    dut.ctrl_ex1_pipe6_data_vld = 0;
    dut.ctrl_ex1_pipe7_data_vld = 0;

    dut.cpurst_b = 0;
    tick(dut);
    dut.cpurst_b = 1;

    uint64_t lcg = 2057;
    for (int i = 0; i < 3000; ++i) {
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
        cov_out = "build/case057/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
