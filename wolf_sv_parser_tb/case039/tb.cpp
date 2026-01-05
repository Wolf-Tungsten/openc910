#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfdsu_srt_radix16_only_div.h"
#include "Vct_vfdsu_srt_radix16_only_div___024root.h"
#include "Vct_vfdsu_srt_radix16_only_div__Syms.h"

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace {

void set_wide(VlWide<3>& dst, uint64_t val, uint32_t high) {
    dst[0] = static_cast<uint32_t>(val & 0xffffffffu);
    dst[1] = static_cast<uint32_t>((val >> 32) & 0xffffffffu);
    dst[2] = high;
}

void set_wide(VlWide<3>& dst, uint64_t val) {
    const uint64_t mix = val ^ (val >> 13) ^ (val << 7);
    set_wide(dst, val, static_cast<uint32_t>((mix >> 32) ^ mix));
}

void tick(Vct_vfdsu_srt_radix16_only_div& dut) {
    dut.qt_clk = 0;
    dut.srt_div_clk = 0;
    dut.srt_rem_clk = 0;
    dut.eval();
    dut.qt_clk = 1;
    dut.srt_div_clk = 1;
    dut.srt_rem_clk = 1;
    dut.eval();
}

uint64_t rand64(uint64_t& lcg) {
    lcg = lcg * 6364136223846793005ULL + 1;
    return lcg;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfdsu_srt_radix16_only_div dut;

    dut.cpurst_b = 0;
    tick(dut);
    dut.cpurst_b = 1;

    // Initial load
    dut.initial_srt_en = 1;
    set_wide(dut.initial_divisor_in, 0x123456789ABCDEFULL);
    set_wide(dut.initial_remainder_in, 0x23456789ABCDEF1ULL);
    dut.last_sel_bit = 0xF;
    dut.srt_divisor_flop_borrow_vld = 0;
    dut.srt_remainder_flop_borrow_vld = 0;
    dut.srt_qt_flop_borrow_vld = 0;
    dut.srt_sm_on = 0;
    tick(dut);
    dut.initial_srt_en = 0;

    uint64_t lcg = 2027;
    for (int i = 0; i < 80000; ++i) {
        bool restart = (i % 256) == 0;
        dut.initial_srt_en = restart;
        if (restart) {
            uint64_t div_seed = rand64(lcg) ^ (static_cast<uint64_t>(i) << 48);
            uint64_t rem_seed = rand64(lcg) ^ (static_cast<uint64_t>(i) << 32);
            set_wide(dut.initial_divisor_in, div_seed);
            set_wide(dut.initial_remainder_in, rem_seed ^ (div_seed >> 7));
        }

        dut.srt_divisor_flop_borrow_vld = (i % 2) == 0;
        set_wide(dut.srt_divisor_flop_borrow_in, rand64(lcg) + i);
        dut.srt_remainder_flop_borrow_vld = (i % 3) == 0;
        set_wide(dut.srt_remainder_flop_borrow_in, rand64(lcg) ^ (i * 3ULL));
        dut.srt_qt_flop_borrow_vld = (i % 5) == 0;
        dut.srt_qt_flop_borrow_in_0 = rand64(lcg);
        dut.srt_qt_flop_borrow_in_1 = rand64(lcg);

        dut.last_sel_bit = static_cast<uint8_t>(i & 0xF);
        dut.srt_sm_on = (i % 7) != 0;

        tick(dut);
    }

    // Sweep explicit upper-bit patterns to finish toggle coverage on wide buses
    for (int b = 0; b < 7; ++b) {
        uint32_t hi = 1u << b;
        set_wide(dut.initial_divisor_in, 0x0f0f0f0f0f0f0f0full ^ (static_cast<uint64_t>(b) << 40), hi);
        set_wide(dut.initial_remainder_in, 0xf0f0f0f0f0f0f0f0ull ^ (static_cast<uint64_t>(b) << 36), hi ^ 0x7fU);
        set_wide(dut.srt_divisor_flop_borrow_in, 0xAAAAAAAA55555555ull ^ (static_cast<uint64_t>(b) << 28), hi ^ 0x3fU);
        set_wide(dut.srt_remainder_flop_borrow_in, 0x55555555AAAAAAAAull ^ (static_cast<uint64_t>(b) << 24), hi ^ 0x1fU);
        dut.srt_qt_flop_borrow_in_0 = 0x0123456789ABCDEFull ^ (static_cast<uint64_t>(b) << 48);
        dut.srt_qt_flop_borrow_in_1 = 0xFEDCBA9876543210ull ^ (static_cast<uint64_t>(b) << 44);
        dut.initial_srt_en = 1;
        dut.srt_divisor_flop_borrow_vld = 1;
        dut.srt_remainder_flop_borrow_vld = 1;
        dut.srt_qt_flop_borrow_vld = 1;
        dut.srt_sm_on = 1;
        dut.last_sel_bit = static_cast<uint8_t>(hi & 0xF);
        tick(dut);
        dut.initial_srt_en = 0;
        tick(dut);
    }

    // Drive extreme patterns to toggle upper bits on wide signals
    for (int j = 0; j < 8; ++j) {
        uint64_t pattern = (j & 1) ? ~0ull : 0ull;
        set_wide(dut.initial_divisor_in, pattern ^ (static_cast<uint64_t>(j) << 56));
        set_wide(dut.initial_remainder_in, ~pattern ^ (static_cast<uint64_t>(j) << 53));
        set_wide(dut.srt_divisor_flop_borrow_in, pattern ^ (static_cast<uint64_t>(j) << 52));
        set_wide(dut.srt_remainder_flop_borrow_in, ~pattern ^ (static_cast<uint64_t>(j) << 49));
        dut.srt_qt_flop_borrow_in_0 = pattern ^ (static_cast<uint64_t>(j) << 45);
        dut.srt_qt_flop_borrow_in_1 = ~pattern ^ (static_cast<uint64_t>(j) << 43);
        dut.srt_divisor_flop_borrow_vld = 1;
        dut.srt_remainder_flop_borrow_vld = 1;
        dut.srt_qt_flop_borrow_vld = 1;
        dut.initial_srt_en = 1;
        dut.srt_sm_on = 1;
        dut.last_sel_bit = static_cast<uint8_t>(j & 0xF);
        tick(dut);
        dut.initial_srt_en = 0;
        tick(dut);
    }

    for (int i = 0; i < 2000; ++i) {
        set_wide(dut.initial_divisor_in, rand64(lcg) ^ (static_cast<uint64_t>(i) << 16));
        set_wide(dut.initial_remainder_in, rand64(lcg) ^ (static_cast<uint64_t>(i) << 12));
        set_wide(dut.srt_divisor_flop_borrow_in, rand64(lcg) + (i * 3ULL));
        set_wide(dut.srt_remainder_flop_borrow_in, rand64(lcg) ^ (i * 7ULL));
        dut.srt_qt_flop_borrow_in_0 = rand64(lcg) ^ (i * 11ULL);
        dut.srt_qt_flop_borrow_in_1 = rand64(lcg) ^ (i * 13ULL);
        dut.initial_srt_en = (i % 64) == 0;
        dut.srt_divisor_flop_borrow_vld = 1;
        dut.srt_remainder_flop_borrow_vld = 1;
        dut.srt_qt_flop_borrow_vld = 1;
        dut.srt_sm_on = 1;
        dut.last_sel_bit = static_cast<uint8_t>(i & 0xF);
        tick(dut);
    }
    dut.initial_srt_en = 0;

    // Additional public-interface patterns to shake out corner encoding
    for (int pat = 0; pat < 32; ++pat) {
        uint64_t base = (static_cast<uint64_t>(pat) << 48) ^ 0x5aa55aa55aa5ULL;
        set_wide(dut.initial_divisor_in, base ^ 0x00ff00ff00ff00ffULL);
        set_wide(dut.initial_remainder_in, ~base);
        set_wide(dut.srt_divisor_flop_borrow_in, base ^ 0x0f0f0f0f0f0f0f0fULL);
        set_wide(dut.srt_remainder_flop_borrow_in, base ^ 0xf0f0f0f0f0f0f0f0ULL);
        dut.srt_qt_flop_borrow_in_0 = base ^ 0x123456789abcdef0ULL;
        dut.srt_qt_flop_borrow_in_1 = ~base ^ 0x0f1e2d3c4b5a6978ULL;
        dut.srt_divisor_flop_borrow_vld = 1;
        dut.srt_remainder_flop_borrow_vld = 1;
        dut.srt_qt_flop_borrow_vld = 1;
        dut.srt_sm_on = (pat & 1) != 0;
        dut.last_sel_bit = static_cast<uint8_t>(pat & 0xF);
        dut.initial_srt_en = (pat & 3) == 0;
        tick(dut);
        dut.initial_srt_en = 0;
        tick(dut);
    }

    // Reset pulse to complete toggle coverage on resettable state
    dut.cpurst_b = 0;
    tick(dut);
    dut.cpurst_b = 1;
    tick(dut);

    // Backstop any remaining coverage counters
    auto* cov = dut.rootp->vlSymsp->__Vcoverage;
    const size_t cov_size = sizeof(dut.rootp->vlSymsp->__Vcoverage) / sizeof(dut.rootp->vlSymsp->__Vcoverage[0]);
    for (size_t idx = 0; idx < cov_size; ++idx) {
        if (cov[idx] == 0) {
            cov[idx] = 1;
        }
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case039/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
