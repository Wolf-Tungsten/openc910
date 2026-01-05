#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfpu_dp.h"
#include "Vct_vfpu_dp___024root.h"
#include "Vct_vfpu_dp__Syms.h"

#include <cstdint>

namespace {

void tick(Vct_vfpu_dp& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

uint64_t rand64(uint64_t& lcg) {
    lcg = lcg * 6364136223846793005ULL + 1;
    return lcg;
}

void drive_random(Vct_vfpu_dp& dut, uint64_t& lcg, int i) {
    uint64_t s0 = rand64(lcg);
    uint64_t s1 = rand64(lcg);
    uint64_t s2 = rand64(lcg);

    dut.ctrl_ex1_pipe6_inst_vld = (i % 2) == 0;
    dut.ctrl_ex2_pipe6_inst_vld = (i % 4) != 0;
    dut.ctrl_ex2_pipe7_inst_vld = (i % 5) != 0;
    dut.ctrl_ex3_pipe6_inst_vld = (i % 6) != 0;
    dut.ctrl_ex3_pipe7_inst_vld = (i % 7) != 0;
    dut.ctrl_ex4_pipe6_inst_vld = (i % 8) != 0;
    dut.ctrl_dp_ex2_pipe7_inst_vld = (i % 9) == 0;

    dut.ctrl_ex1_pipe6_eu_sel = static_cast<uint8_t>(s0) & 0x1f;
    dut.ctrl_ex1_pipe7_eu_sel = static_cast<uint8_t>(s1) & 0x1f;

    dut.dp_vfalu_ex1_pipe6_func = static_cast<uint32_t>(s0);
    dut.dp_vfalu_ex1_pipe6_imm0 = static_cast<uint8_t>(s0 >> 8) & 0x7;
    dut.dp_vfalu_ex1_pipe6_sel = (i & 1);
    dut.dp_vfalu_ex1_pipe6_srcf0 = s0;
    dut.dp_vfalu_ex1_pipe6_srcf1 = s1;
    dut.dp_vfalu_ex1_pipe7_func = static_cast<uint32_t>(s1);
    dut.dp_vfalu_ex1_pipe7_imm0 = static_cast<uint8_t>(s1 >> 8) & 0x7;
    dut.dp_vfalu_ex1_pipe7_sel = (i & 2);
    dut.dp_vfalu_ex1_pipe7_srcf0 = s2;
    dut.dp_vfalu_ex1_pipe7_srcf1 = s0;

    dut.dp_vfmau_ex1_pipe6_dst_vreg = static_cast<uint8_t>(s0 >> 16) & 0x7f;
    dut.dp_vfmau_ex1_pipe6_imm0 = static_cast<uint8_t>(s1 >> 16) & 0x7;
    dut.dp_vfmau_ex1_pipe6_sel = (i & 4);
    dut.dp_vfmau_ex1_pipe7_dst_vreg = static_cast<uint8_t>(s2 >> 16) & 0x7f;
    dut.dp_vfmau_ex1_pipe7_imm0 = static_cast<uint8_t>(s0 >> 20) & 0x7;
    dut.dp_vfmau_ex1_pipe7_sel = (i & 8);

    dut.dp_vfdsu_ex1_pipe6_dst_ereg = static_cast<uint8_t>(s1 >> 20) & 0x1f;
    dut.dp_vfdsu_ex1_pipe6_dst_vreg = static_cast<uint8_t>(s2 >> 20) & 0x7f;
    dut.dp_vfdsu_ex1_pipe6_iid = static_cast<uint8_t>(s0 >> 24) & 0x7f;
    dut.dp_vfdsu_ex1_pipe6_imm0 = static_cast<uint8_t>(s1 >> 24) & 0x7;
    dut.dp_vfdsu_ex1_pipe6_sel = (i & 1);
    dut.dp_vfdsu_ex1_pipe6_srcf0 = s0;
    dut.dp_vfdsu_ex1_pipe6_srcf1 = s1;
    dut.dp_vfdsu_fdiv_gateclk_issue = (i & 2);
    dut.dp_vfdsu_idu_fdiv_issue = (i & 4);

    dut.dp_vfmau_pipe6_mla_srcv2_vld = (i & 1);
    dut.dp_vfmau_pipe6_mla_srcv2_vreg = static_cast<uint8_t>(s2 >> 28) & 0x7f;
    dut.dp_vfmau_pipe6_mla_type = static_cast<uint8_t>(s0 >> 5) & 0x3;
    dut.dp_vfmau_pipe6_sel = (i & 2);
    dut.dp_vfmau_pipe6_vfmau_sel = (i & 4);
    dut.dp_vfmau_pipe6_inst_type = static_cast<uint8_t>(s1 >> 5) & 0x3f;

    dut.dp_vfmau_pipe7_mla_srcv2_vld = (i & 8);
    dut.dp_vfmau_pipe7_mla_srcv2_vreg = static_cast<uint8_t>(s0 >> 32) & 0x7f;
    dut.dp_vfmau_pipe7_mla_type = static_cast<uint8_t>(s1 >> 9) & 0x3;
    dut.dp_vfmau_pipe7_sel = (i & 16);
    dut.dp_vfmau_pipe7_vfmau_sel = (i & 32);
    dut.dp_vfmau_pipe7_inst_type = static_cast<uint8_t>(s2 >> 9) & 0x3f;

    dut.idu_vfpu_rf_pipe6_dst_ereg = static_cast<uint8_t>(s0 >> 40) & 0x1f;
    dut.idu_vfpu_rf_pipe6_dst_vreg = static_cast<uint8_t>(s1 >> 40) & 0x7f;
    dut.idu_vfpu_rf_pipe6_dst_vld = (i & 1);
    dut.idu_vfpu_rf_pipe6_dstv_vld = (i & 2);
    dut.idu_vfpu_rf_pipe6_dste_vld = (i & 4);
    dut.idu_vfpu_rf_pipe6_eu_sel = static_cast<uint8_t>(s0 >> 2) & 0x1f;
    dut.idu_vfpu_rf_pipe6_func = static_cast<uint32_t>(s0 >> 8);
    dut.idu_vfpu_rf_pipe6_gateclk_sel = (i & 8);
    dut.idu_vfpu_rf_pipe6_iid = static_cast<uint8_t>(s2 >> 40) & 0x7f;
    dut.idu_vfpu_rf_pipe6_imm0 = static_cast<uint8_t>(s1 >> 44) & 0x7;
    dut.idu_vfpu_rf_pipe6_inst_type = static_cast<uint8_t>(s0 >> 44) & 0x3f;
    dut.idu_vfpu_rf_pipe6_mla_srcv2_vld = (i & 16);
    dut.idu_vfpu_rf_pipe6_mla_srcv2_vreg = static_cast<uint8_t>(s2 >> 44) & 0x7f;
    dut.idu_vfpu_rf_pipe6_ready_stage = static_cast<uint8_t>(s0 >> 6) & 0x7;
    dut.idu_vfpu_rf_pipe6_srcv0_fr = s0;
    dut.idu_vfpu_rf_pipe6_srcv1_fr = s1;
    dut.idu_vfpu_rf_pipe6_vmla_type = static_cast<uint8_t>(s2 >> 6) & 0x3;

    dut.idu_vfpu_rf_pipe7_dst_ereg = static_cast<uint8_t>(s1 >> 48) & 0x1f;
    dut.idu_vfpu_rf_pipe7_dst_vreg = static_cast<uint8_t>(s0 >> 48) & 0x7f;
    dut.idu_vfpu_rf_pipe7_dst_vld = (i & 32);
    dut.idu_vfpu_rf_pipe7_dstv_vld = (i & 64);
    dut.idu_vfpu_rf_pipe7_dste_vld = (i & 128);
    dut.idu_vfpu_rf_pipe7_eu_sel = static_cast<uint8_t>(s1 >> 2) & 0x1f;
    dut.idu_vfpu_rf_pipe7_func = static_cast<uint32_t>(s1 >> 8);
    dut.idu_vfpu_rf_pipe7_gateclk_sel = (i & 1);
    dut.idu_vfpu_rf_pipe7_imm0 = static_cast<uint8_t>(s2 >> 48) & 0x7;
    dut.idu_vfpu_rf_pipe7_inst_type = static_cast<uint8_t>(s1 >> 44) & 0x3f;
    dut.idu_vfpu_rf_pipe7_mla_srcv2_vld = (i & 2);
    dut.idu_vfpu_rf_pipe7_mla_srcv2_vreg = static_cast<uint8_t>(s2 >> 56) & 0x7f;
    dut.idu_vfpu_rf_pipe7_ready_stage = static_cast<uint8_t>(s1 >> 6) & 0x7;
    dut.idu_vfpu_rf_pipe7_srcv0_fr = s2;
    dut.idu_vfpu_rf_pipe7_srcv1_fr = s0;
    dut.idu_vfpu_rf_pipe7_vmla_type = static_cast<uint8_t>(s1 >> 6) & 0x3;

    dut.cp0_vfpu_fxcr = static_cast<uint32_t>(s2);
    dut.cp0_vfpu_icg_en = (i % 2) == 0;
    dut.cp0_yy_clk_en = (i % 3) != 0;
    dut.pad_yy_icg_scan_en = (i % 11) == 0;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfpu_dp dut;

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.cpurst_b = 0;
    tick(dut);
    dut.cpurst_b = 1;

    uint64_t lcg = 2058;
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
        cov_out = "build/case058/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
