#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfdsu_ff1.h"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

struct Expected {
    uint64_t shift_num;
    uint16_t bin_val;
};

Expected compute_expected(uint64_t frac) {
    if ((frac & ((1ULL << 52) - 1)) == 0) {
        return {0, 0x1fcc};
    }
    int msb = -1;
    for (int i = 51; i >= 0; --i) {
        if (frac & (1ULL << i)) {
            msb = i;
            break;
        }
    }
    if (msb == 51) {
        return {frac & ((1ULL << 52) - 1), 0};
    }
    const uint16_t bin = static_cast<uint16_t>(0x1fff - (50 - msb));
    const int shift = 51 - msb;
    const uint64_t mask52 = (1ULL << 52) - 1;
    const uint64_t shifted = (frac & mask52) << shift;
    return {shifted & mask52, bin};
}

bool run_case(Vct_vfdsu_ff1& dut, uint64_t frac, const std::string& name) {
    dut.frac_num = frac & ((1ULL << 52) - 1);
    dut.eval();
    Expected exp = compute_expected(frac);
    if (dut.fanc_shift_num != exp.shift_num) {
        std::cerr << name << " shift mismatch got 0x" << std::hex << dut.fanc_shift_num << " expected 0x" << exp.shift_num << std::dec << "\n";
        return false;
    }
    if (dut.frac_bin_val != exp.bin_val) {
        std::cerr << name << " bin mismatch got 0x" << std::hex << dut.frac_bin_val << " expected 0x" << exp.bin_val << std::dec << "\n";
        return false;
    }
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfdsu_ff1 dut;

    // Directed: single hot bit for every position plus zero.
    if (!run_case(dut, 0, "all_zero")) return 1;
    for (int i = 0; i < 52; ++i) {
        if (!run_case(dut, 1ULL << i, "bit_" + std::to_string(i))) return 1;
    }
    // Mixed patterns: alternating bits with a defined leading 1.
    std::array<uint64_t, 5> patterns = {
        0xAAAAAAAAAAAAULL & ((1ULL << 52) - 1),
        0x155555555555ULL & ((1ULL << 52) - 1),
        0x0000FFFFFFFFULL,
        0x0F0F0F0F0F0FULL,
        0x00F000000000ULL};
    for (size_t idx = 0; idx < patterns.size(); ++idx) {
        if (!run_case(dut, patterns[idx], "pattern_" + std::to_string(idx))) return 1;
    }

    // Randomized sweep.
    uint64_t lcg = 17;
    auto next = [&]() {
        lcg = lcg * 6364136223846793005ULL + 1;
        return lcg;
    };
    for (int i = 0; i < 1000; ++i) {
        uint64_t frac = next() & ((1ULL << 52) - 1);
        if (!run_case(dut, frac, "rand_" + std::to_string(i))) return 1;
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case033/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
