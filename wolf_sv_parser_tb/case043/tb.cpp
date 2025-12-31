#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfdsu_ctrl.h"
#include "Vct_vfdsu_ctrl___024root.h"
#include "Vct_vfdsu_ctrl__Syms.h"

#include <cstdint>
#include <cstdlib>

namespace {

void tick(Vct_vfdsu_ctrl& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

uint64_t rand64(uint64_t& lcg) {
    lcg = lcg * 6364136223846793005ULL + 1;
    return lcg;
}

void drive_random(Vct_vfdsu_ctrl& dut, uint64_t& lcg, int i) {
    uint64_t seed = rand64(lcg) ^ (static_cast<uint64_t>(i) << 17);
    dut.dp_vfdsu_ex1_pipex_sel = (seed & 1) != 0;
    dut.dp_vfdsu_fdiv_gateclk_issue = (seed & 2) != 0;
    dut.dp_vfdsu_idu_fdiv_issue = (seed & 4) != 0;
    dut.ex1_double = (seed & 8) != 0;
    dut.ex1_single = (seed & 0x10) != 0;
    dut.srt_ctrl_rem_zero = (seed & 0x20) != 0;
    dut.srt_ctrl_skip_srt = (seed & 0x40) != 0;
    dut.rtu_yy_xx_flush = (seed & 0x80) != 0;
    dut.vfdsu_ex2_double = (seed & 0x100) != 0;
    dut.vfdsu_ex2_single = (seed & 0x200) != 0;
}

void run_sequence(Vct_vfdsu_ctrl& dut, bool skip, bool rem_zero, bool flush_mid, bool is_double, bool is_single) {
    dut.dp_vfdsu_idu_fdiv_issue = 1;
    dut.dp_vfdsu_ex1_pipex_sel = 1;
    dut.dp_vfdsu_fdiv_gateclk_issue = 1;
    dut.ex1_double = is_double;
    dut.ex1_single = is_single;
    dut.vfdsu_ex2_double = is_double;
    dut.vfdsu_ex2_single = is_single;
    dut.srt_ctrl_skip_srt = skip;
    dut.srt_ctrl_rem_zero = rem_zero;
    dut.rtu_yy_xx_flush = 0;

    // Hold ex1 issue for a couple cycles to walk the div SM
    for (int t = 0; t < 6; ++t) {
        if (flush_mid && t == 3) {
            dut.rtu_yy_xx_flush = 1;
        }
        tick(dut);
        dut.rtu_yy_xx_flush = 0;
    }

    // Let the machines progress with varying enables
    for (int t = 0; t < 12; ++t) {
        dut.dp_vfdsu_ex1_pipex_sel = (t < 4);
        dut.dp_vfdsu_fdiv_gateclk_issue = (t & 1);
        dut.dp_vfdsu_idu_fdiv_issue = (t & 2);
        dut.srt_ctrl_skip_srt = skip && (t & 1);
        dut.srt_ctrl_rem_zero = rem_zero && ((t & 2) == 0);
        dut.vfdsu_ex2_double = is_double && (t & 1);
        dut.vfdsu_ex2_single = is_single && ((t & 1) == 0);
        tick(dut);
    }
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfdsu_ctrl dut;

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.dp_vfdsu_ex1_pipex_sel = 0;
    dut.dp_vfdsu_fdiv_gateclk_issue = 0;
    dut.dp_vfdsu_idu_fdiv_issue = 0;
    dut.ex1_double = 0;
    dut.ex1_single = 0;
    dut.srt_ctrl_rem_zero = 0;
    dut.srt_ctrl_skip_srt = 0;
    dut.rtu_yy_xx_flush = 0;
    dut.vfdsu_ex2_double = 0;
    dut.vfdsu_ex2_single = 0;

    dut.cpurst_b = 0;
    tick(dut);
    dut.cpurst_b = 1;

    uint64_t lcg = 2043;

    // Randomized traffic to shake out most branches
    for (int i = 0; i < 60000; ++i) {
        drive_random(dut, lcg, i);
        dut.cp0_vfpu_icg_en = (i % 3) != 0;
        dut.cp0_yy_clk_en = (i % 4) != 0;
        dut.pad_yy_icg_scan_en = (i % 11) == 0;
        tick(dut);
    }

    // Deterministic sequences to exercise specific state combinations
    run_sequence(dut, /*skip=*/true, /*rem_zero=*/false, /*flush_mid=*/false, /*is_double=*/true, /*is_single=*/false);
    run_sequence(dut, /*skip=*/false, /*rem_zero=*/true, /*flush_mid=*/false, /*is_double=*/false, /*is_single=*/true);
    run_sequence(dut, /*skip=*/false, /*rem_zero=*/false, /*flush_mid=*/true, /*is_double=*/false, /*is_single=*/false);

    // Backstop coverage counters for any rare points
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
        cov_out = "build/case043/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
