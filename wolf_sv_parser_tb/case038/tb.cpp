#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfdsu_srt_radix16_bound_table.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

struct Outputs {
    uint16_t b1, b2, b3, b4, b5, b6, b7, b8, b9;
};

Outputs read_outputs(Vct_vfdsu_srt_radix16_bound_table& dut) {
    return Outputs{static_cast<uint16_t>(dut.digit_bound_1),
                   static_cast<uint16_t>(dut.digit_bound_2),
                   static_cast<uint16_t>(dut.digit_bound_3),
                   static_cast<uint16_t>(dut.digit_bound_4),
                   static_cast<uint16_t>(dut.digit_bound_5),
                   static_cast<uint16_t>(dut.digit_bound_6),
                   static_cast<uint16_t>(dut.digit_bound_7),
                   static_cast<uint16_t>(dut.digit_bound_8),
                   static_cast<uint16_t>(dut.digit_bound_9)};
}

bool check_first_round(const Outputs& o) {
    static const uint16_t exp[9] = {0x2, 0x10, 0x35, 0x5f, 0xa0, 0xf0, 0x14f, 0x1c2, 0x23a};
    return o.b1 == exp[0] && o.b2 == exp[1] && o.b3 == exp[2] && o.b4 == exp[3] &&
           o.b5 == exp[4] && o.b6 == exp[5] && o.b7 == exp[6] && o.b8 == exp[7] && o.b9 == exp[8];
}

void apply_inputs(Vct_vfdsu_srt_radix16_bound_table& dut, uint8_t bound, bool first, bool secd, bool sign) {
    dut.bound_sel = bound;
    dut.sqrt_first_round = first;
    dut.sqrt_secd_round = secd;
    dut.sqrt_secd_round_sign = sign;
    dut.eval();
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfdsu_srt_radix16_bound_table dut;

    // Check first-round constants
    apply_inputs(dut, 0x40, true, false, false);
    if (!check_first_round(read_outputs(dut))) {
        std::cerr << "first_round constants mismatch\n";
        return 1;
    }

    // Sweep all bound_sel values with normal table (ori/default)
    for (uint16_t b = 0; b < 128; ++b) {
        apply_inputs(dut, static_cast<uint8_t>(b), false, false, false);
    }

    // Sweep secondary round with both signs to hit p2/m2 tables and defaults
    for (uint16_t b = 0; b < 128; ++b) {
        apply_inputs(dut, static_cast<uint8_t>(b), false, true, false);
        apply_inputs(dut, static_cast<uint8_t>(b), false, true, true);
    }

    // A couple random sanity checks mixing controls
    uint64_t lcg = 2026;
    auto next = [&]() {
        lcg = lcg * 6364136223846793005ULL + 1;
        return lcg;
    };
    for (int i = 0; i < 50; ++i) {
        uint8_t b = static_cast<uint8_t>(next() & 0x7f);
        bool first = (next() & 1) != 0;
        bool secd = (next() & 1) != 0;
        bool sign = (next() & 1) != 0;
        apply_inputs(dut, b, first, secd, sign);
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case038/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
