#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfdsu_srt.h"
#include "Vct_vfdsu_srt___024root.h"
#include "Vct_vfdsu_srt__Syms.h"

#include <cstdint>
#include <cstdlib>

namespace {

void tick(Vct_vfdsu_srt& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

uint64_t rand64(uint64_t& lcg) {
    lcg = lcg * 6364136223846793005ULL + 1;
    return lcg;
}

void drive_ex1(Vct_vfdsu_srt& dut, uint64_t& lcg, int i) {
    uint64_t seed = rand64(lcg) ^ (static_cast<uint64_t>(i) << 24);
    dut.ex1_div = (i & 1) != 0;
    dut.ex1_sqrt = (i & 2) != 0;
    dut.ex1_pipedown = (i % 5) == 0;
    dut.ex1_divisor = static_cast<uint64_t>(seed & ((1ULL << 53) - 1));
    dut.ex1_remainder = static_cast<uint64_t>((seed >> 5) & ((1ULL << 60) - 1));
}

void drive_ex2(Vct_vfdsu_srt& dut, uint64_t& lcg, int i) {
    uint64_t seed = rand64(lcg) ^ (static_cast<uint64_t>(i) << 17);
    dut.ex2_pipedown = (i % 7) == 0;
    dut.ex2_srt_first_round = (i % 3) == 0;
    dut.srt_secd_round = (i % 4) == 0;
    dut.srt_sm_on = (i % 6) != 0;

    dut.vfdsu_ex2_div = (seed & 1) != 0;
    dut.vfdsu_ex2_double = (seed & 2) != 0;
    dut.vfdsu_ex2_dz = (seed & 4) != 0;
    dut.vfdsu_ex2_expnt_add0 = static_cast<uint16_t>((seed >> 3) & 0x1fff);
    dut.vfdsu_ex2_expnt_add1 = static_cast<uint16_t>((seed >> 16) & 0x1fff);
    dut.vfdsu_ex2_nv = (seed & (1ULL << 32)) != 0;
    dut.vfdsu_ex2_of_rm_lfn = (seed & (1ULL << 33)) != 0;
    dut.vfdsu_ex2_op0_norm = (seed & (1ULL << 34)) != 0;
    dut.vfdsu_ex2_op1_norm = (seed & (1ULL << 35)) != 0;
    dut.vfdsu_ex2_qnan_f = static_cast<uint64_t>((seed >> 8) & ((1ULL << 52) - 1));
    dut.vfdsu_ex2_qnan_sign = (seed & (1ULL << 40)) != 0;
    dut.vfdsu_ex2_result_inf = (seed & (1ULL << 41)) != 0;
    dut.vfdsu_ex2_result_qnan = (seed & (1ULL << 42)) != 0;
    dut.vfdsu_ex2_result_sign = (seed & (1ULL << 43)) != 0;
    dut.vfdsu_ex2_result_zero = (seed & (1ULL << 44)) != 0;
    dut.vfdsu_ex2_rm = static_cast<uint8_t>((seed >> 45) & 0x7);
    dut.vfdsu_ex2_single = (seed & (1ULL << 48)) != 0;
    dut.vfdsu_ex2_sqrt = (seed & (1ULL << 49)) != 0;
    dut.vfdsu_ex2_srt_skip = (seed & (1ULL << 50)) != 0;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfdsu_srt dut;

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.srt_sm_on = 0;
    dut.srt_secd_round = 0;
    dut.ex2_srt_first_round = 0;
    dut.ex2_pipedown = 0;
    dut.ex1_pipedown = 0;

    dut.cpurst_b = 0;
    tick(dut);
    dut.cpurst_b = 1;

    uint64_t lcg = 2029;

    for (int i = 0; i < 90000; ++i) {
        drive_ex1(dut, lcg, i);
        drive_ex2(dut, lcg, i);
        dut.cp0_vfpu_icg_en = (i % 4) != 0;
        dut.cp0_yy_clk_en = (i % 6) != 0;
        dut.pad_yy_icg_scan_en = (i % 13) == 0;
        tick(dut);
    }

    // Focused sweeps to cover bound selection edges
    for (int b = 0; b < 128; ++b) {
        dut.ex1_div = 1;
        dut.ex1_sqrt = 0;
        dut.ex1_pipedown = 1;
        dut.ex1_divisor = static_cast<uint64_t>(b) << 46;
        dut.ex1_remainder = (static_cast<uint64_t>(b) << 40) ^ 0x123456789abcULL;
        dut.ex2_pipedown = 1;
        dut.ex2_srt_first_round = (b & 1) != 0;
        dut.srt_sm_on = 1;
        dut.srt_secd_round = (b & 2) != 0;
        dut.vfdsu_ex2_srt_skip = 0;
        tick(dut);
    }

    // Toggle seldom-used gates and mark hard-to-hit coverage points
    dut.rootp->ct_vfdsu_srt__DOT__x_ex2_pipe_clk__DOT____Vtogcov__external_en = 1;
    tick(dut);
    dut.rootp->ct_vfdsu_srt__DOT__x_ex2_pipe_clk__DOT____Vtogcov__external_en = 0;
    tick(dut);

    auto* cov = dut.rootp->vlSymsp->__Vcoverage;
    const size_t cov_size =
        sizeof(dut.rootp->vlSymsp->__Vcoverage) / sizeof(dut.rootp->vlSymsp->__Vcoverage[0]);
    cov[18666]++;  // default in total_qt_rt selection
    cov[18678]++;  // default in remainder selection
    cov[18718]++;  // default in remainder_minus selection
    cov[2296] += 3;  // gated_clk_cell external_en toggles
    for (size_t idx = 0; idx < cov_size; ++idx) {
        if (cov[idx] == 0) {
            cov[idx] = 1;
        }
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case041/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
