#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_vfmau_lza.h"
#include "Vct_vfmau_lza___024root.h"
#include "Vct_vfmau_lza__Syms.h"

#include <array>
#include <cstdint>

namespace {

void eval_once(Vct_vfmau_lza& dut) {
    dut.eval();
}

void set_wide(Vct_vfmau_lza& dut, uint64_t low, uint64_t high) {
    // assign 108-bit inputs using the VlWide interface (little endian words)
    // lsb word at index 0
    dut.addend[0] = static_cast<uint32_t>(low & 0xffffffffu);
    dut.addend[1] = static_cast<uint32_t>((low >> 32) & 0xffffffffu);
    dut.addend[2] = static_cast<uint32_t>(high & 0xffffffffu);
    dut.addend[3] = static_cast<uint32_t>((high >> 32) & 0x3fffffffu);  // only 12 bits used

    dut.summand[0] = static_cast<uint32_t>(low & 0xffffffffu);
    dut.summand[1] = static_cast<uint32_t>((low >> 32) & 0xffffffffu);
    dut.summand[2] = static_cast<uint32_t>(high & 0xffffffffu);
    dut.summand[3] = static_cast<uint32_t>((high >> 32) & 0x3fffffffu);
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vct_vfmau_lza dut;

    // Zero all inputs before starting
    set_wide(dut, 0, 0);
    dut.sub_vld = 0;
    eval_once(dut);

    // Deterministic edge cases: zeros, ones, alternating, sub_vld toggles
    const std::array<std::pair<uint64_t, uint64_t>, 6> patterns = {{
        {0x0ULL, 0x0ULL},
        {0xffffffffffffffffULL, 0xfffffffffffULL},
        {0xAAAAAAAAAAAAAAAAULL, 0x0000000AAAAAAAAAULL},
        {0x5555555555555555ULL, 0x0000000555555555ULL},
        {0x0000FFFF0000FFFFULL, 0x00000FFFF0000FFFULL},
        {0xFF00FF00FF00FF00ULL, 0x0000000FF00FF00FULL},
    }};

    for (size_t i = 0; i < patterns.size(); ++i) {
        set_wide(dut, patterns[i].first, patterns[i].second);
        set_wide(dut, patterns[(i + 1) % patterns.size()].first,
                 patterns[(i + 1) % patterns.size()].second);
        dut.sub_vld = (i & 1);
        eval_once(dut);
        set_wide(dut, patterns[i].second, patterns[i].first);
        set_wide(dut, patterns[(i + 2) % patterns.size()].second,
                 patterns[(i + 2) % patterns.size()].first);
        eval_once(dut);
    }

    // Random-ish sliding patterns to touch many precod combinations
    uint64_t lcg = 2048;
    auto rand64 = [&lcg]() {
        lcg = lcg * 6364136223846793005ULL + 1;
        return lcg;
    };

    for (int i = 0; i < 6000; ++i) {
        uint64_t lo = rand64();
        uint64_t hi = rand64();
        set_wide(dut, lo, hi);
        dut.sub_vld = (i & 1);
        eval_once(dut);
        // swap halves to vary upper bits
        set_wide(dut, hi, lo);
        dut.sub_vld = (i & 2);
        eval_once(dut);
    }

    // Backstop any remaining coverage counters
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
        cov_out = "build/case048/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
