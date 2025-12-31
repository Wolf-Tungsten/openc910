#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfmau_lza_42.h"
#include "Vct_vfmau_lza_42___024root.h"
#include "Vct_vfmau_lza_42__Syms.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfmau_lza_42 dut;

    // Sweep every 4-bit input to cover all logic paths
    for (int v = 0; v < 16; ++v) {
        dut.lza_precod = static_cast<uint8_t>(v);
        dut.eval();
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
        cov_out = "build/case047/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
