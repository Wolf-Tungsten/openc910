#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fspu_ctrl.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

void tick(Vct_fspu_ctrl& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fspu_ctrl dut;

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    // Apply reset.
    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.cpurst_b = 0;
    dut.dp_vfalu_ex1_pipex_sel = 0;
    tick(dut);
    if (!expect(dut.ex1_pipedown == 0, "ex1_pipedown should start low")) {
        return 1;
    }
    if (!expect(dut.ex2_pipedown == 0 && dut.ex3_pipedown == 0, "reset failed to clear pipeline flags")) {
        return 1;
    }

    // Release reset and drive a valid pulse.
    dut.cpurst_b = 1;
    dut.pad_yy_icg_scan_en = 1;
    tick(dut);

    dut.dp_vfalu_ex1_pipex_sel = 0x1;  // pulse ex1_pipedown
    dut.eval();
    if (!expect(dut.ex1_pipedown == 1, "ex1_pipedown should follow sel[0]")) {
        return 1;
    }
    tick(dut);
    if (!expect(dut.ex2_pipedown == 1, "ex2_pipedown should latch ex1 pulse")) {
        return 1;
    }
    if (!expect(dut.ex3_pipedown == 0, "ex3_pipedown should remain low initially")) {
        return 1;
    }

    dut.dp_vfalu_ex1_pipex_sel = 0;
    tick(dut);
    if (!expect(dut.ex2_pipedown == 0, "ex2_pipedown should clear after pulse")) {
        return 1;
    }
    if (!expect(dut.ex3_pipedown == 1, "ex3_pipedown should capture ex2 pulse")) {
        return 1;
    }
    tick(dut);
    if (!expect(dut.ex3_pipedown == 0, "ex3_pipedown should drop after pulse")) {
        return 1;
    }

    // Toggle gates to cover icg inputs.
    dut.cp0_yy_clk_en = 0;
    dut.cp0_vfpu_icg_en = 0;
    dut.dp_vfalu_ex1_pipex_sel = 1;
    tick(dut);
    dut.cp0_yy_clk_en = 1;
    dut.cp0_vfpu_icg_en = 1;
    dut.dp_vfalu_ex1_pipex_sel = 1;
    tick(dut);
    dut.dp_vfalu_ex1_pipex_sel = 0;
    tick(dut);

    // Mid-run reset to exercise both edges.
    dut.cpurst_b = 0;
    tick(dut);
    if (!expect(dut.ex2_pipedown == 0 && dut.ex3_pipedown == 0, "mid-run reset failed")) {
        return 1;
    }
    dut.cpurst_b = 1;
    dut.dp_vfalu_ex1_pipex_sel = 1;
    tick(dut);

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case021/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
