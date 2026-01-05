#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfdsu_ff1.h"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

constexpr uint64_t kFracMask = (1ULL << 52) - 1;

struct Expected {
    uint64_t shift_num;
    uint16_t bin_val;
};

Expected compute_expected(uint64_t frac) {
    if ((frac & kFracMask) == 0) {
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
        return {frac & kFracMask, 0};
    }
    const uint16_t bin = static_cast<uint16_t>(0x1fff - (50 - msb));
    const int shift = 51 - msb;
    const uint64_t shifted = (frac & kFracMask) << shift;
    return {shifted & kFracMask, bin};
}

bool run_case(Vct_vfdsu_ff1& dut, uint64_t frac, const std::string& name, int& total, int& failed) {
    ++total;
    const uint64_t masked = frac & kFracMask;
    dut.frac_num = masked;
    dut.eval();
    Expected exp = compute_expected(masked);
    const bool shift_ok = dut.fanc_shift_num == exp.shift_num;
    const bool bin_ok = dut.frac_bin_val == exp.bin_val;
    if (!shift_ok || !bin_ok) {
        ++failed;
        std::cerr << "[FAIL] " << name << " shift=0x" << std::hex << dut.fanc_shift_num << " (exp 0x" << exp.shift_num
                  << ") bin=0x" << dut.frac_bin_val << " (exp 0x" << exp.bin_val << ")" << std::dec << "\n";
        return false;
    }
    std::cout << "[PASS] " << name << " shift=0x" << std::hex << dut.fanc_shift_num << " bin=0x" << dut.frac_bin_val << std::dec << "\n";
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfdsu_ff1 dut;
    int total = 0;
    int failed = 0;

    // Directed: single hot bit for every position plus zero.
    run_case(dut, 0, "all_zero", total, failed);
    for (int i = 0; i < 52; ++i) {
        run_case(dut, 1ULL << i, "bit_" + std::to_string(i), total, failed);
    }
    // Mixed patterns: alternating bits with a defined leading 1.
    std::array<uint64_t, 5> patterns = {
        0xAAAAAAAAAAAAULL & ((1ULL << 52) - 1),
        0x155555555555ULL & ((1ULL << 52) - 1),
        0x0000FFFFFFFFULL,
        0x0F0F0F0F0F0FULL,
        0x00F000000000ULL};
    for (size_t idx = 0; idx < patterns.size(); ++idx) {
        run_case(dut, patterns[idx], "pattern_" + std::to_string(idx), total, failed);
    }

    // Randomized sweep.
    uint64_t lcg = 17;
    auto next = [&]() {
        lcg = lcg * 6364136223846793005ULL + 1;
        return lcg;
    };
    for (int i = 0; i < 1000; ++i) {
        uint64_t frac = next() & ((1ULL << 52) - 1);
        run_case(dut, frac, "rand_" + std::to_string(i), total, failed);
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case033/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    const int passed = total - failed;
    if (failed == 0) {
        std::cout << "[RESULT] PASS (" << passed << "/" << total << ")\n";
        return 0;
    }
    std::cerr << "[RESULT] FAIL (" << passed << "/" << total << ")\n";
    return 1;
}
