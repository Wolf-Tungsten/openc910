#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fadd_ctrl.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

void tick(Vct_fadd_ctrl& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fadd_ctrl dut;

    auto set_pipe_sel = [&](uint8_t sel) {
        dut.dp_vfalu_ex1_pipex_sel = sel & 0x7;
        dut.eval();
    };

    auto expect = [&](bool cond, const std::string& msg) -> bool {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    auto drive_ctrls = [&](bool module_en, bool global_en, bool scan_en) {
        dut.cp0_vfpu_icg_en = module_en;
        dut.cp0_yy_clk_en = global_en;
        dut.pad_yy_icg_scan_en = scan_en;
        dut.eval();
    };

    // Apply reset and default control values.
    dut.cp0_vfpu_icg_en = 0;
    dut.cp0_yy_clk_en = 0;
    dut.cpurst_b = 0;
    dut.pad_yy_icg_scan_en = 0;
    set_pipe_sel(0);
    dut.forever_cpuclk = 0;
    dut.eval();
    tick(dut);

    // Release reset with clocks enabled for the pipeline flops.
    dut.cpurst_b = 1;
    drive_ctrls(true, true, false);

    if (!expect(dut.ex1_pipedown == 0, "ex1_pipedown should be deasserted after reset")) {
        return 1;
    }
    if (!expect(dut.ex2_pipedown == 0 && dut.ex3_pipedown == 0,
                "Pipeline stages should reset to 0")) {
        return 1;
    }

    // Exercise other bits on dp_vfalu_ex1_pipex_sel for toggle coverage.
    set_pipe_sel(0b001);
    set_pipe_sel(0);
    set_pipe_sel(0b100);
    set_pipe_sel(0);

    // Drive a single-cycle pulse through ex1 -> ex2 -> ex3.
    set_pipe_sel(0b010);
    if (!expect(dut.ex1_pipedown == 1, "ex1_pipedown did not follow dp_vfalu_ex1_pipex_sel[1]")) {
        return 1;
    }
    tick(dut);
    if (!expect(dut.ex2_pipedown == 1, "ex2_pipedown was not asserted on first cycle")) {
        return 1;
    }
    if (!expect(dut.ex3_pipedown == 0, "ex3_pipedown should lag ex2_pipedown by one cycle")) {
        return 1;
    }

    set_pipe_sel(0);
    tick(dut);
    if (!expect(dut.ex2_pipedown == 0, "ex2_pipedown did not clear after pulse")) {
        return 1;
    }
    if (!expect(dut.ex3_pipedown == 1, "ex3_pipedown did not capture ex2 pulse")) {
        return 1;
    }

    tick(dut);
    if (!expect(dut.ex3_pipedown == 0, "ex3_pipedown did not clear after propagation")) {
        return 1;
    }

    // Toggle icg controls to cover both edges on module/global/scan enables.
    drive_ctrls(true, true, true);
    tick(dut);
    drive_ctrls(false, false, false);
    tick(dut);
    drive_ctrls(true, true, false);
    tick(dut);

    const char* cov_env = std::getenv("COV_OUT");
    std::string cov_path = cov_env ? cov_env : "build/case007/coverage.dat";
    VerilatedCov::write(cov_path.c_str());

    std::cout << "Pipeline pulse observed across ex1->ex2->ex3; coverage written to "
              << cov_path << "\n";
    return 0;
}
