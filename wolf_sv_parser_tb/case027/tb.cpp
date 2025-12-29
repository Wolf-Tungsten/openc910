#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfalu_dp_pipe7.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_vfalu_dp_pipe7 dut;

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    // fadd only.
    dut.dp_vfalu_ex1_pipex_sel = 0b010;
    dut.fadd_mfvr_cmp_result = 0xCAFEBABEDEADBEEFULL;
    dut.fspu_mfvr_data = 0x1111222233334444ULL;
    dut.fadd_ereg_ex3_forward_r_vld = 1;
    dut.fadd_ereg_ex3_result = 0x1E;
    dut.fcnvt_ereg_forward_r_vld = 0;
    dut.fadd_forward_r_vld = 1;
    dut.fadd_forward_result = 0xAAAABBBBCCCCDDDDULL;
    dut.fcnvt_forward_r_vld = 0;
    dut.fspu_forward_r_vld = 0;
    dut.eval();
    if (!expect(dut.pipex_dp_ex1_vfalu_mfvr_data == dut.fadd_mfvr_cmp_result, "mfvr mux should pick fadd path")) return 1;
    if (!expect(dut.pipex_dp_ex3_vfalu_freg_data == dut.fadd_forward_result, "freg mux should pick fadd")) return 1;
    if (!expect(dut.pipex_dp_ex3_vfalu_ereg_data == dut.fadd_ereg_ex3_result, "ereg mux should pick fadd")) return 1;

    // fcnvt only.
    dut.dp_vfalu_ex1_pipex_sel = 0;
    dut.fadd_ereg_ex3_forward_r_vld = 0;
    dut.fcnvt_ereg_forward_r_vld = 1;
    dut.fcnvt_ereg_forward_result = 0x1B;
    dut.fadd_forward_r_vld = 0;
    dut.fcnvt_forward_r_vld = 1;
    dut.fcnvt_forward_result = 0x0123456789ABCDEFULL;
    dut.fspu_forward_r_vld = 0;
    dut.eval();
    if (!expect(dut.pipex_dp_ex3_vfalu_freg_data == dut.fcnvt_forward_result, "freg mux should pick fcnvt")) return 1;
    if (!expect(dut.pipex_dp_ex3_vfalu_ereg_data == dut.fcnvt_ereg_forward_result, "ereg mux should pick fcnvt")) return 1;

    // fspu only.
    dut.dp_vfalu_ex1_pipex_sel = 0b001;
    dut.fcnvt_ereg_forward_r_vld = 0;
    dut.fadd_forward_r_vld = 0;
    dut.fcnvt_forward_r_vld = 0;
    dut.fspu_forward_r_vld = 1;
    dut.fspu_forward_result = 0x2222444466668888ULL;
    dut.eval();
    if (!expect(dut.pipex_dp_ex1_vfalu_mfvr_data == dut.fspu_mfvr_data, "mfvr mux should pick fspu")) return 1;
    if (!expect(dut.pipex_dp_ex3_vfalu_freg_data == dut.fspu_forward_result, "freg mux should pick fspu")) return 1;
    if (!expect(dut.pipex_dp_ex3_vfalu_ereg_data == 0, "ereg mux default mismatch")) return 1;

    // Invalid multiple-valid combination: exercise default branch without checking value (don't-care in RTL).
    dut.dp_vfalu_ex1_pipex_sel = 0b011;
    dut.fadd_forward_r_vld = 1;
    dut.fcnvt_forward_r_vld = 1;
    dut.fspu_forward_r_vld = 1;
    dut.fadd_forward_result = 0xF0F0F0F0F0F0F0F0ULL;
    dut.fcnvt_forward_result = 0x0F0F0F0F0F0F0F0FULL;
    dut.fspu_forward_result = 0xAAAAAAAAAAAAAAA0ULL;
    dut.eval();

    // Random toggles for coverage.
    uint64_t lcg = 7;
    auto next = [&]() {
        lcg = lcg * 2862933555777941757ULL + 3037000493ULL;
        return lcg;
    };
    for (int i = 0; i < 64; ++i) {
        dut.dp_vfalu_ex1_pipex_sel = i & 0x7;
        dut.fadd_mfvr_cmp_result = next();
        dut.fspu_mfvr_data = next();
        dut.fadd_ereg_ex3_result = next() & 0x1F;
        dut.fcnvt_ereg_forward_result = next() & 0x1F;
        dut.fadd_forward_result = next();
        dut.fcnvt_forward_result = next();
        dut.fspu_forward_result = next();
        dut.fadd_forward_r_vld = (i & 1);
        dut.fcnvt_forward_r_vld = (i >> 1) & 1;
        dut.fspu_forward_r_vld = (i >> 2) & 1;
        dut.fadd_ereg_ex3_forward_r_vld = (i >> 3) & 1;
        dut.fcnvt_ereg_forward_r_vld = (i >> 4) & 1;
        dut.eval();
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case027/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
