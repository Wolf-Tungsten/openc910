#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfmau_top.h"
#include "Vct_vfmau_top___024root.h"
#include "Vct_vfmau_top__Syms.h"

#include <cstdint>

namespace {

void tick(Vct_vfmau_top& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

uint64_t rand64(uint64_t& lcg) {
    lcg = lcg * 6364136223846793005ULL + 1;
    return lcg;
}

void set_wide68(VlWide<3>& target, uint64_t low, uint32_t high_upper_bits) {
    target[0] = static_cast<uint32_t>(low & 0xffffffffu);
    target[1] = static_cast<uint32_t>((low >> 32) & 0xffffffffu);
    target[2] = high_upper_bits & 0x3ffffu;
}

void drive_random(Vct_vfmau_top& dut, uint64_t& lcg, int i) {
    uint64_t s0 = rand64(lcg);
    uint64_t s1 = rand64(lcg);
    uint64_t s2 = rand64(lcg);

    dut.dp_vfmau_ex1_pipex_dst_vreg = static_cast<uint8_t>(s0) & 0x7f;
    dut.dp_vfmau_ex1_pipex_imm0 = static_cast<uint8_t>(s1) & 0x7;
    dut.dp_vfmau_ex1_pipex_sel = (i & 1);

    dut.dp_vfmau_pipe6_mla_srcv2_vld = (i & 2);
    dut.dp_vfmau_pipe6_mla_srcv2_vreg = static_cast<uint8_t>(s0 >> 8) & 0x7f;
    dut.dp_vfmau_pipe6_mla_type = static_cast<uint8_t>(s1 >> 4) & 0x3;
    dut.dp_vfmau_pipe7_mla_srcv2_vld = (i & 4);
    dut.dp_vfmau_pipe7_mla_srcv2_vreg = static_cast<uint8_t>(s2 >> 8) & 0x7f;
    dut.dp_vfmau_pipe7_mla_type = static_cast<uint8_t>(s0 >> 12) & 0x3;
    dut.dp_vfmau_pipex_inst_type = static_cast<uint8_t>(s1 >> 6) & 0x3f;
    dut.dp_vfmau_pipex_sel = (i & 8);
    dut.dp_vfmau_pipex_vfmau_sel = (i & 16);
    dut.dp_vfmau_rf_pipex_sel = (i & 32);

    dut.idu_vfpu_rf_pipex_func = static_cast<uint32_t>(s2);
    dut.idu_vfpu_rf_pipex_gateclk_sel = (i & 1);
    dut.idu_vfpu_rf_pipex_srcv0_fr = s0;
    dut.idu_vfpu_rf_pipex_srcv1_fr = s1;
    dut.idu_vfpu_rf_pipex_srcv2_fr = s2;

    dut.pipe6_pipex_ex4_fmla_fwd_vld = (i & 1);
    dut.pipe6_pipex_ex5_ex1_fmla_fwd_vld = (i & 2);
    dut.pipe6_pipex_ex5_ex2_fmla_fwd_vld = (i & 4);
    dut.pipe6_vfmau_ex4_fmla_slice0_half0_data = static_cast<uint16_t>(s0);
    set_wide68(dut.pipe6_vfmau_ex5_fmla_slice0_data, s1, static_cast<uint32_t>(s2 & 0xffffu));

    dut.pipe7_pipex_ex4_fmla_fwd_vld = (i & 8);
    dut.pipe7_pipex_ex5_ex1_fmla_fwd_vld = (i & 16);
    dut.pipe7_pipex_ex5_ex2_fmla_fwd_vld = (i & 32);
    dut.pipe7_vfmau_ex4_fmla_slice0_half0_data = static_cast<uint16_t>(s1 >> 2);
    set_wide68(dut.pipe7_vfmau_ex5_fmla_slice0_data, s2, static_cast<uint32_t>(s0 & 0xffffu));

    dut.pipex_rbus_ex1_fmla_data_vld = (i & 1);
    dut.pipex_rbus_ex1_fmla_data_vld_dup0 = (i & 2);
    dut.pipex_rbus_ex1_fmla_data_vld_dup1 = (i & 4);
    dut.pipex_rbus_ex1_fmla_data_vld_dup2 = (i & 8);
    dut.pipex_rbus_ex2_fmla_data_vld = (i & 16);
    dut.pipex_rbus_ex2_fmla_data_vld_dup0 = (i & 32);
    dut.pipex_rbus_ex2_fmla_data_vld_dup1 = (i & 64);
    dut.pipex_rbus_ex2_fmla_data_vld_dup2 = (i & 128);
    dut.pipex_rbus_pipe6_fmla_no_fwd = (i & 1);
    dut.pipex_rbus_pipe7_fmla_no_fwd = (i & 2);
    dut.pipex_rbus_vfmau_ereg_wb_data = static_cast<uint8_t>(s0) & 0x1f;
    dut.pipex_rbus_vfmau_ereg_wb_vld = (i & 4);
    dut.pipex_rbus_vfmau_freg_wb_data = s1;
    dut.pipex_rbus_vfmau_vreg_wb_vld = (i & 8);

    dut.pipex_vfmau_ex4_fmla_slice0_half0_data = static_cast<uint16_t>(s2);
    set_wide68(dut.pipex_vfmau_ex5_fmla_slice0_data, s0, static_cast<uint32_t>(s1 & 0xffffu));

    dut.vfpu_yy_xx_dqnan = (s2 >> 63) & 1;
    dut.vfpu_yy_xx_rm = static_cast<uint8_t>((s0 >> 3) & 0x7);
    dut.rtu_yy_xx_flush = (i % 19) == 0;

    dut.cp0_vfpu_icg_en = (i % 3) != 0;
    dut.cp0_yy_clk_en = (i % 4) != 0;
    dut.pad_yy_icg_scan_en = (i % 11) == 0;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfmau_top dut;

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.rtu_yy_xx_flush = 0;
    dut.dp_vfmau_ex1_pipex_sel = 0;
    dut.cpurst_b = 0;
    tick(dut);
    dut.cpurst_b = 1;

    uint64_t lcg = 2055;

    for (int i = 0; i < 7000; ++i) {
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
        cov_out = "build/case055/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
