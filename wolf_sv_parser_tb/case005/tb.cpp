#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fadd_onehot_sel_d.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fadd_onehot_sel_d dut;

    constexpr unsigned kWidth = 54;
    constexpr uint64_t kMask = (uint64_t{1} << kWidth) - 1;

    auto shift_result = [](uint64_t data, unsigned shift, uint64_t mask) -> uint64_t {
        unsigned __int128 wide = static_cast<unsigned __int128>(data) & mask;
        wide <<= shift;
        wide &= mask;
        return static_cast<uint64_t>(wide);
    };

    std::vector<uint64_t> patterns = {
        0ULL,
        kMask,
        0x15555555555555ULL & kMask,
        0x0AAAAAAAAAAAAAULL & kMask,
        uint64_t{1} << 53,
        0x123456789ABCDEULL & kMask
    };

    std::array<bool, kWidth> onehot_hits{};
    bool zero_hit = false;
    bool default_hit = false;

    dut.data_in = 0;
    dut.onehot = 0;
    dut.eval();

    for (uint64_t data : patterns) {
        data &= kMask;
        dut.data_in = data;
        for (unsigned bit = 0; bit < kWidth; ++bit) {
            uint64_t onehot = uint64_t{1} << bit;
            dut.onehot = onehot;
            dut.eval();

            unsigned shift = 53 - bit;
            uint64_t expected = shift_result(data, shift, kMask);
            uint64_t actual = dut.result & kMask;
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
    dut.onehot = (uint64_t{1} << 3) | (uint64_t{1} << 7);
    dut.eval();
    default_hit = true;

    const char* cov_env = std::getenv("COV_OUT");
    std::string cov_path = cov_env ? cov_env : "build/case005/coverage.dat";
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
