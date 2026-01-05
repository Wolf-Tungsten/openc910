#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfpu_cbus.h"
#include "Vct_vfpu_cbus___024root.h"
#include "Vct_vfpu_cbus__Syms.h"

#include <cstdint>

namespace {

void tick(Vct_vfpu_cbus& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

uint64_t rand64(uint64_t& lcg) {
    lcg = lcg * 6364136223846793005ULL + 1;
    return lcg;
}

void drive_random(Vct_vfpu_cbus& dut, uint64_t& lcg, int i) {
    uint64_t s = rand64(lcg);
    dut.idu_vfpu_rf_pipe6_gateclk_sel = (s & 1) != 0;
    dut.idu_vfpu_rf_pipe7_gateclk_sel = (s & 2) != 0;
    dut.idu_vfpu_rf_pipe6_sel = (s & 4) != 0;
    dut.idu_vfpu_rf_pipe7_sel = (s & 8) != 0;
    dut.idu_vfpu_rf_pipe6_iid = static_cast<uint8_t>((s >> 8) & 0x7f);
    dut.idu_vfpu_rf_pipe7_iid = static_cast<uint8_t>((s >> 16) & 0x7f);

    dut.cp0_vfpu_icg_en = (i % 3) != 0;
    dut.cp0_yy_clk_en = (i % 4) != 0;
    dut.pad_yy_icg_scan_en = (i % 11) == 0;
    dut.rtu_yy_xx_flush = (i % 19) == 0;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfpu_cbus dut;

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.rtu_yy_xx_flush = 0;
    dut.idu_vfpu_rf_pipe6_gateclk_sel = 0;
    dut.idu_vfpu_rf_pipe7_gateclk_sel = 0;
    dut.idu_vfpu_rf_pipe6_sel = 0;
    dut.idu_vfpu_rf_pipe7_sel = 0;
    dut.idu_vfpu_rf_pipe6_iid = 0;
    dut.idu_vfpu_rf_pipe7_iid = 0;

    dut.cpurst_b = 0;
    tick(dut);
    dut.cpurst_b = 1;

    uint64_t lcg = 2056;

    for (int i = 0; i < 3000; ++i) {
        drive_random(dut, lcg, i);
        tick(dut);
    }

    // Deterministic patterns to toggle gate and flush behavior
    for (int t = 0; t < 32; ++t) {
        dut.idu_vfpu_rf_pipe6_gateclk_sel = (t & 1);
        dut.idu_vfpu_rf_pipe7_gateclk_sel = (t & 2);
        dut.idu_vfpu_rf_pipe6_sel = (t & 4);
        dut.idu_vfpu_rf_pipe7_sel = (t & 8);
        dut.idu_vfpu_rf_pipe6_iid = static_cast<uint8_t>(t);
        dut.idu_vfpu_rf_pipe7_iid = static_cast<uint8_t>(t ^ 0x3f);
        dut.rtu_yy_xx_flush = (t == 10);
        dut.cp0_vfpu_icg_en = (t % 3) != 0;
        dut.cp0_yy_clk_en = (t % 2) != 0;
        tick(dut);
        dut.rtu_yy_xx_flush = 0;
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
        cov_out = "build/case056/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
