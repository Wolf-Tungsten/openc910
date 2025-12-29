#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fcnvt_ctrl.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

void tick(Vct_fcnvt_ctrl& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fcnvt_ctrl dut;

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.cpurst_b = 0;
    dut.dp_vfalu_ex1_pipex_sel = 0;
    tick(dut);

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    if (!expect(dut.ex2_pipedown == 0 && dut.ex3_pipedown == 0, "reset did not clear")) {
        return 1;
    }

    dut.cpurst_b = 1;
    dut.pad_yy_icg_scan_en = 1;
    tick(dut);

    dut.dp_vfalu_ex1_pipex_sel = 0x4;  // ex1_pipedown asserted
    dut.eval();
    if (!expect(dut.ex1_pipedown == 1, "ex1_pipedown should follow sel[2]")) {
        return 1;
    }
    tick(dut);
    if (!expect(dut.ex2_pipedown == 1, "ex2_pipedown should latch ex1 pulse")) {
        return 1;
    }
    if (!expect(dut.ex3_pipedown == 0, "ex3_pipedown should stay low initially")) {
        return 1;
    }

    dut.dp_vfalu_ex1_pipex_sel = 0x0;  // drop ex1
    dut.eval();
    tick(dut);
    if (!expect(dut.ex2_pipedown == 0, "ex2_pipedown should clear after pulse")) {
        return 1;
    }
    if (!expect(dut.ex3_pipedown == 1, "ex3_pipedown should capture ex2 pulse")) {
        return 1;
    }

    dut.dp_vfalu_ex1_pipex_sel = 0x0;
    dut.eval();
    tick(dut);
    if (!expect(dut.ex3_pipedown == 0, "ex3_pipedown should clear")) {
        return 1;
    }

    // Stress gating enables and chained pulses.
    dut.cp0_yy_clk_en = 0;
    dut.cp0_vfpu_icg_en = 0;
    dut.dp_vfalu_ex1_pipex_sel = 0x4;
    tick(dut);
    dut.cp0_yy_clk_en = 1;
    dut.cp0_vfpu_icg_en = 1;
    dut.dp_vfalu_ex1_pipex_sel = 0x4;
    tick(dut);  // capture with gated clocks back on
    dut.dp_vfalu_ex1_pipex_sel = 0x0;
    tick(dut);

    // Toggle lower pipex bits and pad scan to cover vector bits.
    dut.pad_yy_icg_scan_en = 1;
    dut.dp_vfalu_ex1_pipex_sel = 0x1;
    tick(dut);
    dut.dp_vfalu_ex1_pipex_sel = 0x2;
    tick(dut);
    dut.dp_vfalu_ex1_pipex_sel = 0x3;
    tick(dut);
    dut.pad_yy_icg_scan_en = 0;
    tick(dut);

    // Re-assert reset mid-sim to exercise both reset and non-reset edges.
    dut.cpurst_b = 0;
    tick(dut);
    if (!expect(dut.ex2_pipedown == 0 && dut.ex3_pipedown == 0, "reset mid-sim failed")) {
        return 1;
    }
    dut.cpurst_b = 1;
    dut.dp_vfalu_ex1_pipex_sel = 0x7;
    tick(dut);

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case020/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
