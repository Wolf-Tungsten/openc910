#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fadd_onehot_sel_h.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fadd_onehot_sel_h dut;

    constexpr unsigned kWidth = 12;
    constexpr uint16_t kMask = (1u << kWidth) - 1;

    auto shift_result = [](uint16_t data, unsigned shift, uint16_t mask) -> uint16_t {
        uint32_t wide = static_cast<uint32_t>(data) & mask;
        wide <<= shift;
        wide &= mask;
        return static_cast<uint16_t>(wide);
    };

    std::vector<uint16_t> patterns = {
        0u,
        kMask,
        static_cast<uint16_t>(0x555 & kMask),
        static_cast<uint16_t>(0xAAA & kMask),
        static_cast<uint16_t>(1u << 11),
        static_cast<uint16_t>(0xACE & kMask)
    };

    std::array<bool, kWidth> onehot_hits{};
    bool zero_hit = false;
    bool default_hit = false;

    dut.data_in = 0;
    dut.onehot = 0;
    dut.eval();

    for (uint16_t data : patterns) {
        data &= kMask;
        dut.data_in = data;
        for (unsigned bit = 0; bit < kWidth; ++bit) {
            uint16_t onehot = static_cast<uint16_t>(1u << bit);
            dut.onehot = onehot;
            dut.eval();

            unsigned shift = (kWidth - 1) - bit;
            uint16_t expected = shift_result(data, shift, kMask);
            uint16_t actual = static_cast<uint16_t>(dut.result) & kMask;
            if (actual != expected) {
                std::cerr << "Mismatch data=0x" << std::hex << data
                          << " onehot bit=" << std::dec << bit
                          << " dut=0x" << std::hex << actual
                          << " expected=0x" << expected << std::dec << "\n";
                return 1;
            }
            onehot_hits[bit] = true;
        }
    }

    dut.data_in = kMask;
    dut.onehot = 0;
    dut.eval();
    zero_hit = true;
    if ((dut.result & kMask) != 0) {
        std::cerr << "Zero onehot produced non-zero result: 0x"
                  << std::hex << (dut.result & kMask) << std::dec << "\n";
        return 1;
    }

    // Exercise default branch with an invalid multi-bit onehot; result is don't care.
    dut.onehot = static_cast<uint16_t>((1u << 2) | (1u << 7));
    dut.eval();
    default_hit = true;

    const char* cov_env = std::getenv("COV_OUT");
    std::string cov_path = cov_env ? cov_env : "build/case006/coverage.dat";
    VerilatedCov::write(cov_path.c_str());

    auto count_hits = [](const auto& container) {
        return std::count(container.begin(), container.end(), true);
    };

    std::cout << "onehot branches hit: " << count_hits(onehot_hits)
              << "/" << kWidth
              << ", zero branch hit: " << (zero_hit ? "yes" : "no")
              << ", default branch hit: " << (default_hit ? "yes" : "no")
              << "\n";
    std::cout << "Coverage written to " << cov_path << "\n";
    return 0;
}
