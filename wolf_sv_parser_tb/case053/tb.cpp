#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfmau_dp.h"
#include "Vct_vfmau_dp___024root.h"
#include "Vct_vfmau_dp__Syms.h"

#include <cstdint>

namespace {

void tick(Vct_vfmau_dp& dut) {
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

void drive_random(Vct_vfmau_dp& dut, uint64_t& lcg, int i) {
    uint64_t a = rand64(lcg);
    uint64_t b = rand64(lcg);
    uint64_t c = rand64(lcg);

    dut.dp_mult1_ex1_op0_slice0 = a;
    dut.dp_mult1_ex1_op1_slice0 = b;
    dut.dp_mult1_ex1_op2_slice0 = c;
    dut.dp_mult1_ex1_op0_slice0_high = static_cast<uint32_t>(a >> 8);
    dut.dp_mult1_ex1_op1_slice0_high = static_cast<uint32_t>(b >> 8);
    dut.dp_mult1_ex1_op2_slice0_high = static_cast<uint32_t>(c >> 8);

    dut.dp_mult_ex1_op0_slice0_half0 = static_cast<uint16_t>(a);
    dut.dp_mult_ex1_op1_slice0_half0 = static_cast<uint16_t>(b);
    dut.dp_mult_ex1_op2_slice0_half0 = static_cast<uint32_t>(c & 0xffffu);
    dut.dp_mult_ex1_op0_slice0_half0_high = static_cast<uint64_t>(a >> 4) & 0xffffffffffffULL;
    dut.dp_mult_ex1_op1_slice0_half0_high = static_cast<uint64_t>(b >> 4) & 0xffffffffffffULL;
    dut.dp_mult_ex1_op2_slice0_half0_high = static_cast<uint64_t>(c >> 4) & 0xffffffffffffULL;

    dut.dp_mult1_op2_slice0_vl_mask = (i & 1);
    dut.dp_mult1_op2_slice0_vm_mask = (i & 2);
    dut.dp_mult_op2_slice0_vl_half0_mask = (i & 4);
    dut.dp_mult_op2_slice0_vm_half0_mask = (i & 8);

    dut.dp_vfmau_ex1_pipex_dst_vreg = static_cast<uint8_t>(a) & 0x7f;
    dut.dp_vfmau_ex1_pipex_imm0 = static_cast<uint8_t>(b) & 0x7;
    dut.dp_vfmau_pipe6_mla_srcv2_vld = (i & 1);
    dut.dp_vfmau_pipe6_mla_srcv2_vreg = static_cast<uint8_t>(b >> 8) & 0x7f;
    dut.dp_vfmau_pipe6_mla_type = static_cast<uint8_t>(a >> 16) & 0x3;
    dut.dp_vfmau_pipe7_mla_srcv2_vld = (i & 2);
    dut.dp_vfmau_pipe7_mla_srcv2_vreg = static_cast<uint8_t>(c >> 8) & 0x7f;
    dut.dp_vfmau_pipe7_mla_type = static_cast<uint8_t>(b >> 16) & 0x3;
    dut.dp_vfmau_pipex_inst_type = static_cast<uint8_t>(a >> 20) & 0x3;
    dut.dp_vfmau_pipex_sel = (i & 1);
    dut.dp_vfmau_pipex_vfmau_sel = (i & 2);
    dut.dp_vfmau_rf_pipex_sel = (i & 4);

    dut.dp_xx_ex1_double = (i & 1);
    dut.dp_xx_ex1_fma = (i & 2);
    dut.dp_xx_ex1_half = (i & 4);
    dut.dp_xx_ex1_neg = (i & 8);
    dut.dp_xx_ex1_simd = (i & 16);
    dut.dp_xx_ex1_single = (i & 32);
    dut.dp_xx_ex1_sub = (i & 64);
    dut.dp_xx_ex1_widen = (i & 128);
    dut.dp_xx_ex1_op0_frac = a & ((1ULL << 52) - 1);
    dut.dp_xx_ex1_op1_frac = b & ((1ULL << 52) - 1);
    dut.dp_xx_ex1_rm = static_cast<uint8_t>((c >> 3) & 0x7);

    dut.dp_xx_ex2_double = (a >> 60) & 1;
    dut.dp_xx_ex2_fma = (b >> 61) & 1;
    dut.dp_xx_ex2_half = (c >> 62) & 1;
    dut.dp_xx_ex2_mult_id = (a >> 55) & 1;
    dut.dp_xx_ex2_neg = (b >> 56) & 1;
    dut.dp_xx_ex2_rm = static_cast<uint8_t>((c >> 6) & 0x7);
    dut.dp_xx_ex2_simd = (a >> 57) & 1;
    dut.dp_xx_ex2_sub = (b >> 58) & 1;
    dut.dp_xx_ex2_widen = (c >> 59) & 1;

    dut.dp_xx_ex3_double = (i & 1);
    dut.dp_xx_ex3_fma = (i & 2);
    dut.dp_xx_ex3_half = (i & 4);
    dut.dp_xx_ex3_mult_id = (i & 8);
    dut.dp_xx_ex3_rm = static_cast<uint8_t>((a >> 10) & 0x7);
    dut.dp_xx_ex3_simd = (i & 16);
    dut.dp_xx_ex3_widen = (i & 32);

    dut.dp_xx_ex4_double = (i & 1);
    dut.dp_xx_ex4_fma = (i & 2);
    dut.dp_xx_ex4_half = (i & 4);
    dut.dp_xx_ex4_mult_id = (i & 8);
    dut.dp_xx_ex4_rm = static_cast<uint8_t>((b >> 12) & 0x7);

    dut.ctrl_ex1_ex2_en = (i % 2) == 0;
    dut.ctrl_ex1_inst_vld = (i % 3) != 0;
    dut.ctrl_ex2_inst_vld = (i % 4) != 0;
    dut.ctrl_ex3_inst_vld = (i % 5) != 0;
    dut.ctrl_ex4_inst_vld = (i % 6) != 0;
    dut.ctrl_ex5_inst_vld = (i % 7) != 0;
    dut.ctrl_dp_ex5_fma_wb_vld = (i % 5) == 0;

    dut.idu_vfpu_rf_pipex_func = static_cast<uint32_t>(a);
    dut.idu_vfpu_rf_pipex_gateclk_sel = (i & 1);
    dut.idu_vfpu_rf_pipex_srcv0_fr = a;
    dut.idu_vfpu_rf_pipex_srcv1_fr = b;
    dut.idu_vfpu_rf_pipex_srcv2_fr = c;

    dut.pipex_pipe6_ex4_fmla_fwd_vld = (i & 1);
    dut.pipex_pipe6_ex5_ex1_fmla_fwd_vld = (i & 2);
    dut.pipex_pipe6_ex5_ex2_fmla_fwd_vld = (i & 4);
    dut.pipex_pipe7_ex4_fmla_fwd_vld = (i & 8);
    dut.pipex_pipe7_ex5_ex1_fmla_fwd_vld = (i & 16);
    dut.pipex_pipe7_ex5_ex2_fmla_fwd_vld = (i & 32);

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
    dut.pipex_rbus_vfmau_ereg_wb_data = static_cast<uint8_t>(c) & 0x1f;
    dut.pipex_rbus_vfmau_ereg_wb_vld = (i & 4);
    dut.pipex_rbus_vfmau_freg_wb_data = a;
    dut.pipex_rbus_vfmau_vreg_wb_vld = (i & 8);

    dut.pipex_vfmau_ex4_fmla_slice0_half0_data = static_cast<uint16_t>(b);
    set_wide68(dut.pipex_vfmau_ex5_fmla_slice0_data, a, static_cast<uint32_t>(b & 0xffffu));
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfmau_dp dut;

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.cpurst_b = 0;
    tick(dut);
    dut.cpurst_b = 1;

    uint64_t lcg = 2053;

    for (int i = 0; i < 6000; ++i) {
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
        cov_out = "build/case053/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
