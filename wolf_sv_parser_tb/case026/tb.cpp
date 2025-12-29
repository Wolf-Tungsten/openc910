#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfalu_dp_pipe6.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_vfalu_dp_pipe6 dut;

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    // Scenario 1: fadd forward only.
    dut.dp_vfalu_ex1_pipex_sel = 0b010;
    dut.fadd_mfvr_cmp_result = 0x1111222233334444ULL;
    dut.fspu_mfvr_data = 0x5555666677778888ULL;
    dut.fadd_ereg_ex3_forward_r_vld = 1;
    dut.fadd_ereg_ex3_result = 0x1A;
    dut.fadd_forward_r_vld = 1;
    dut.fadd_forward_result = 0xDEADBEEFCAFEBABEULL;
    dut.fspu_forward_r_vld = 0;
    dut.fspu_forward_result = 0;
    dut.eval();

    if (!expect(dut.pipex_dp_ex1_vfalu_mfvr_data == dut.fadd_mfvr_cmp_result, "mfvr mux should pick fadd path")) return 1;
    if (!expect(dut.pipex_dp_ex3_vfalu_freg_data == dut.fadd_forward_result, "freg mux should pick fadd")) return 1;
    if (!expect(dut.pipex_dp_ex3_vfalu_ereg_data == dut.fadd_ereg_ex3_result, "ereg mux should pick fadd")) return 1;

    // Scenario 2: fspu forward only.
    dut.dp_vfalu_ex1_pipex_sel = 0b001;
    dut.fadd_ereg_ex3_forward_r_vld = 0;
    dut.fadd_forward_r_vld = 0;
    dut.fspu_forward_r_vld = 1;
    dut.fspu_forward_result = 0x123456789ABCDEF0ULL;
    dut.eval();
    if (!expect(dut.pipex_dp_ex1_vfalu_mfvr_data == dut.fspu_mfvr_data, "mfvr mux should pick fspu path")) return 1;
    if (!expect(dut.pipex_dp_ex3_vfalu_freg_data == dut.fspu_forward_result, "freg mux should pick fspu")) return 1;
    if (!expect(dut.pipex_dp_ex3_vfalu_ereg_data == 0, "ereg mux should clear when no fadd valid")) return 1;

    // Scenario 3: neither valid -> X state collapses to unknowns; just ensure mux selects defaults.
    dut.dp_vfalu_ex1_pipex_sel = 0;
    dut.fadd_forward_r_vld = 0;
    dut.fspu_forward_r_vld = 0;
    dut.fadd_ereg_ex3_forward_r_vld = 0;
    dut.eval();
    if (!expect(dut.pipex_dp_ex1_vfalu_mfvr_data == 0, "mfvr mux default mismatch")) return 1;

    // Toggle through a few random mixes for coverage.
    uint64_t lcg = 3;
    auto next = [&]() {
        lcg = lcg * 1103515245ULL + 12345ULL;
        return lcg;
    };
    for (int i = 0; i < 64; ++i) {
        dut.dp_vfalu_ex1_pipex_sel = i & 0x7;
        dut.fadd_mfvr_cmp_result = next();
        dut.fspu_mfvr_data = next();
        dut.fadd_forward_result = next();
        dut.fspu_forward_result = next();
        dut.fadd_ereg_ex3_result = next() & 0x1F;
        dut.fadd_forward_r_vld = (i & 1);
        dut.fspu_forward_r_vld = (i >> 1) & 1;
        dut.fadd_ereg_ex3_forward_r_vld = (i >> 2) & 1;
        dut.eval();
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case026/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
