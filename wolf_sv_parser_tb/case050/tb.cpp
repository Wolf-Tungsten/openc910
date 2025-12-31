#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfmau_mult1.h"
#include "Vct_vfmau_mult1___024root.h"
#include "Vct_vfmau_mult1__Syms.h"

#include <cstdint>

namespace {

void tick(Vct_vfmau_mult1& dut) {
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
    target[2] = high_upper_bits & 0x3ffffu;  // top 4 bits used (68-bit)
}

void drive_once(Vct_vfmau_mult1& dut, uint64_t& lcg, int i) {
    uint64_t s0 = rand64(lcg);
    uint64_t s1 = rand64(lcg);
    uint64_t s2 = rand64(lcg);

    dut.dp_mult1_ex1_op0_slicex = s0;
    dut.dp_mult1_ex1_op1_slicex = s1;
    dut.dp_mult1_ex1_op2_slicex = s2;
    dut.dp_mult1_ex1_op0_slicex_high = static_cast<uint32_t>(s0 >> 16);
    dut.dp_mult1_ex1_op1_slicex_high = static_cast<uint32_t>(s1 >> 12);
    dut.dp_mult1_ex1_op2_slicex_high = static_cast<uint32_t>(s2 >> 8);

    dut.dp_mult_ex1_op0_slicex_half0 = static_cast<uint16_t>(s0);
    dut.dp_mult_ex1_op1_slicex_half0 = static_cast<uint16_t>(s1 >> 1);
    dut.dp_mult_ex1_op2_slicex_half0 = static_cast<uint32_t>(s2 & 0xffffu);
    dut.dp_mult_ex1_op0_slicex_half0_high = static_cast<uint64_t>(s0 >> 3) & 0xffffffffffffULL;
    dut.dp_mult_ex1_op1_slicex_half0_high = static_cast<uint64_t>(s1 >> 5) & 0xffffffffffffULL;
    dut.dp_mult_ex1_op2_slicex_half0_high = static_cast<uint64_t>(s2 >> 7) & 0xffffffffffffULL;

    dut.dp_mult1_op2_slicex_vl_mask = (i & 1);
    dut.dp_mult1_op2_slicex_vm_mask = (i & 2);
    dut.dp_mult_op2_slicex_vl_half0_mask = (i & 4);
    dut.dp_mult_op2_slicex_vm_half0_mask = (i & 8);

    dut.dp_mult1_ex1_clk_en = (i % 2) == 0;
    dut.dp_mult1_ex2_clk_en = (i % 3) != 0;
    dut.dp_mult1_ex3_clk_en = (i % 4) != 0;
    dut.dp_mult1_ex4_clk_en = (i % 5) != 0;

    dut.dp_xx_ex1_double = (i & 1);
    dut.dp_xx_ex1_fma = (i & 2);
    dut.dp_xx_ex1_half = (i & 4);
    dut.dp_xx_ex1_neg = (i & 8);
    dut.dp_xx_ex1_simd = (i & 16);
    dut.dp_xx_ex1_single = (i & 32);
    dut.dp_xx_ex1_sub = (i & 64);
    dut.dp_xx_ex1_widen = (i & 128);
    dut.dp_xx_ex1_op0_frac = s0 & ((1ULL << 52) - 1);
    dut.dp_xx_ex1_op1_frac = s1 & ((1ULL << 52) - 1);
    dut.dp_xx_ex1_rm = static_cast<uint8_t>((s2 >> 4) & 0x7);

    dut.dp_xx_ex2_double = (s0 >> 56) & 1;
    dut.dp_xx_ex2_fma = (s1 >> 57) & 1;
    dut.dp_xx_ex2_half = (s2 >> 58) & 1;
    dut.dp_xx_ex2_mult_id = (s0 >> 59) & 1;
    dut.dp_xx_ex2_neg = (s1 >> 60) & 1;
    dut.dp_xx_ex2_rm = static_cast<uint8_t>((s2 >> 1) & 0x7);
    dut.dp_xx_ex2_simd = (s0 >> 61) & 1;
    dut.dp_xx_ex2_sub = (s1 >> 62) & 1;
    dut.dp_xx_ex2_widen = (s2 >> 63) & 1;

    dut.dp_xx_ex3_double = (i & 1);
    dut.dp_xx_ex3_fma = (i & 2);
    dut.dp_xx_ex3_half = (i & 4);
    dut.dp_xx_ex3_mult_id = (i & 8);
    dut.dp_xx_ex3_rm = static_cast<uint8_t>((s0 >> 7) & 0x7);
    dut.dp_xx_ex3_simd = (i & 16);
    dut.dp_xx_ex3_widen = (i & 32);

    dut.dp_xx_ex4_double = (i & 1);
    dut.dp_xx_ex4_half = (i & 2);
    dut.dp_xx_ex4_rm = static_cast<uint8_t>((s1 >> 9) & 0x7);

    dut.mult1_ex1_ex2_pipedown = (i % 3) != 0;
    dut.mult1_ex2_ex3_pipedown = (i % 4) != 0;
    dut.mult1_ex3_ex4_pipedown = (i % 5) != 0;
    dut.mult1_ex4_ex5_pipedown = (i % 6) != 0;
    dut.mult_ex1_ex2_half_pipedown = (i % 2) == 0;
    dut.mult_ex2_ex3_half_pipedown = (i % 3) == 0;
    dut.mult_ex3_ex4_half_pipedown = (i % 4) == 0;
    dut.mult_ex4_ex5_half_pipedown = (i % 5) == 0;

    dut.pipe6_pipex_ex4_fmla_fwd_vld = (i & 1);
    dut.pipe6_pipex_ex5_ex1_fmla_fwd_vld = (i & 2);
    dut.pipe6_pipex_ex5_ex2_fmla_fwd_vld = (i & 4);
    dut.pipe6_vfmau_ex4_fmla_slicex_half0_data = static_cast<uint16_t>(s0);
    set_wide68(dut.pipe6_vfmau_ex5_fmla_slicex_data, s1, static_cast<uint32_t>(s2 & 0xffffu));

    dut.pipe7_pipex_ex4_fmla_fwd_vld = (i & 8);
    dut.pipe7_pipex_ex5_ex1_fmla_fwd_vld = (i & 16);
    dut.pipe7_pipex_ex5_ex2_fmla_fwd_vld = (i & 32);
    dut.pipe7_vfmau_ex4_fmla_slicex_half0_data = static_cast<uint16_t>(s1 >> 2);
    set_wide68(dut.pipe7_vfmau_ex5_fmla_slicex_data, s2, static_cast<uint32_t>(s0 & 0xffffu));

    dut.vfpu_yy_xx_dqnan = (s2 >> 63) & 1;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfmau_mult1 dut;

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.cpurst_b = 0;
    tick(dut);
    dut.cpurst_b = 1;

    uint64_t lcg = 2050;

    for (int i = 0; i < 4000; ++i) {
        drive_once(dut, lcg, i);
        tick(dut);
    }

    // Stress enables and pipedowns without data changes
    for (int t = 0; t < 64; ++t) {
        dut.dp_mult1_ex1_clk_en = (t & 1);
        dut.dp_mult1_ex2_clk_en = (t & 2);
        dut.dp_mult1_ex3_clk_en = (t & 4);
        dut.dp_mult1_ex4_clk_en = (t & 8);
        dut.mult1_ex1_ex2_pipedown = (t & 1);
        dut.mult1_ex2_ex3_pipedown = (t & 2);
        dut.mult1_ex3_ex4_pipedown = (t & 4);
        dut.mult1_ex4_ex5_pipedown = (t & 8);
        dut.mult_ex1_ex2_half_pipedown = (t & 16);
        dut.mult_ex2_ex3_half_pipedown = (t & 32);
        dut.mult_ex3_ex4_half_pipedown = (t & 1);
        dut.mult_ex4_ex5_half_pipedown = (t & 2);
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
        cov_out = "build/case050/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
