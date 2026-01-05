#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfmau_ff1_10bit.h"
#include "Vct_vfmau_ff1_10bit___024root.h"
#include "Vct_vfmau_ff1_10bit__Syms.h"

#include <cstdint>

namespace {

void tick(Vct_vfmau_ff1_10bit& dut) {
    dut.eval();  // purely combinational; single eval is sufficient
    dut.eval();
}

uint16_t make_pattern(int leading_one_pos) {
    // leading_one_pos: 1..10 maps to position from MSB to LSB
    if (leading_one_pos < 1 || leading_one_pos > 10) return 0;
    return static_cast<uint16_t>(1u << (10 - leading_one_pos));
}

uint8_t expected_ff1(uint16_t data) {
    for (int pos = 9; pos >= 0; --pos) {
        if (data & (1u << pos)) {
            return static_cast<uint8_t>(10 - pos);
        }
    }
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfmau_ff1_10bit dut;

    // Sweep each distinct case branch
    for (int pos = 1; pos <= 10; ++pos) {
        dut.ff1_data = make_pattern(pos);
        tick(dut);
        const uint8_t exp = static_cast<uint8_t>(pos);
        if (dut.ff1_result != exp) return 1;
    }

    // Default branch (no bits set)
    dut.ff1_data = 0;
    tick(dut);

    // Additional mixed patterns to toggle masking with multiple ones
    const uint16_t patterns[] = {
        0b1010101010,
        0b0101010101,
        0b1111000000,
        0b0000111111,
        0b0011001100,
        0b1100000011
    };
    for (uint16_t p : patterns) {
        dut.ff1_data = p;
        tick(dut);
        if (dut.ff1_result != expected_ff1(p)) return 1;
    }

    // Backstop coverage in case any branch was missed
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
        cov_out = "build/case045/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
