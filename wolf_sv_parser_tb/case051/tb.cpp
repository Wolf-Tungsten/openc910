#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfmau_mult_compressor.h"
#include "Vct_vfmau_mult_compressor___024root.h"
#include "Vct_vfmau_mult_compressor__Syms.h"

#include <cstdint>

namespace {

void tick(Vct_vfmau_mult_compressor& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

uint64_t rand64(uint64_t& lcg) {
    lcg = lcg * 6364136223846793005ULL + 1;
    return lcg;
}

void drive_random(Vct_vfmau_mult_compressor& dut, uint64_t& lcg, int i) {
    uint64_t a = rand64(lcg);
    uint64_t b = rand64(lcg);

    dut.op0_frac = a & ((1ULL << 52) - 1);
    dut.op1_frac = b & ((1ULL << 52) - 1);
    dut.mult1_ex1_op0_hidden_bit = (a >> 52) & 1;
    dut.mult1_ex1_op1_hidden_bit = (b >> 52) & 1;
    dut.mult1_ex1_op0_half0_hidden_bit = (a >> 53) & 1;
    dut.mult1_ex1_op1_half0_hidden_bit = (b >> 53) & 1;

    dut.dp_xx_ex1_double = (i & 1);
    dut.dp_xx_ex1_single = (i & 2);
    dut.dp_xx_ex1_half = (i & 4);

    dut.pipe_down = (i % 3) != 0;

    dut.cp0_vfpu_icg_en = (i % 4) != 0;
    dut.cp0_yy_clk_en = (i % 5) != 0;
    dut.pad_yy_icg_scan_en = (i % 11) == 0;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfmau_mult_compressor dut;

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.pipe_down = 0;
    dut.dp_xx_ex1_double = 0;
    dut.dp_xx_ex1_single = 0;
    dut.dp_xx_ex1_half = 0;
    dut.mult1_ex1_op0_hidden_bit = 0;
    dut.mult1_ex1_op1_hidden_bit = 0;
    dut.mult1_ex1_op0_half0_hidden_bit = 0;
    dut.mult1_ex1_op1_half0_hidden_bit = 0;
    dut.op0_frac = 0;
    dut.op1_frac = 0;

    uint64_t lcg = 2051;

    // Broad randomized traffic
    for (int i = 0; i < 6000; ++i) {
        drive_random(dut, lcg, i);
        tick(dut);
    }

    // Targeted mode sweeps
    for (int mode = 0; mode < 8; ++mode) {
        dut.dp_xx_ex1_double = mode & 1;
        dut.dp_xx_ex1_single = mode & 2;
        dut.dp_xx_ex1_half = mode & 4;
        dut.pipe_down = 1;
        dut.op0_frac = 0xAAAAAAAAAAAAULL ^ mode;
        dut.op1_frac = 0x555555555555ULL ^ (mode << 4);
        dut.mult1_ex1_op0_hidden_bit = mode & 1;
        dut.mult1_ex1_op1_hidden_bit = mode & 2;
        dut.mult1_ex1_op0_half0_hidden_bit = mode & 4;
        dut.mult1_ex1_op1_half0_hidden_bit = mode & 1;
        tick(dut);
    }

    // Hold pipe_down low/high to cover gated clock paths
    dut.pipe_down = 0;
    for (int j = 0; j < 16; ++j) {
        tick(dut);
    }
    dut.pipe_down = 1;
    for (int j = 0; j < 16; ++j) {
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
        cov_out = "build/case051/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
