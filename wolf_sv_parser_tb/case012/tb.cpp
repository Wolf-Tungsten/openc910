#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fcnvt_dtoh_sh.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

uint64_t mask_bits(uint64_t value, int hi, int lo) {
    if (hi < lo) {
        return 0;
    }
    int width = hi - lo + 1;
    if (width >= 64) {
        return value;
    }
    return (value >> lo) & ((uint64_t(1) << width) - 1);
}

struct Scenario {
    uint16_t cnt;
    uint64_t src;
};

struct Expected {
    uint16_t f_v;
    uint64_t f_x;
};

Expected compute_expected(uint16_t cnt, uint64_t raw_src) {
    uint64_t src = raw_src & ((uint64_t(1) << 52) - 1);
    Expected e{};

    struct Pattern {
        uint16_t cnt;
        int const_width;
        int slice_hi;
        int slice_lo;
        int zero_pad;
    };
    static const std::vector<Pattern> patterns = {
        {0x3f0, 2, 51, 43, 11}, {0x3ef, 3, 51, 44, 10}, {0x3ee, 4, 51, 45, 9},
        {0x3ed, 5, 51, 46, 8},  {0x3ec, 6, 51, 47, 7},  {0x3eb, 7, 51, 48, 6},
        {0x3ea, 8, 51, 49, 5},  {0x3e9, 9, 51, 50, 4},  {0x3e8, 10, 51, 51, 3},
        {0x3e7, 11, -1, -1, 2},
    };

    for (const auto& p : patterns) {
        if (cnt == p.cnt) {
            uint16_t prefix = static_cast<uint16_t>(1u << (11 - p.const_width));
            uint16_t slice = 0;
            if (p.slice_hi >= 0) {
                slice = static_cast<uint16_t>(mask_bits(src, p.slice_hi, p.slice_lo));
            }
            e.f_v = prefix | slice;

            uint64_t remainder = 0;
            if (p.slice_hi >= 0) {
                int rem_hi = p.slice_lo - 1;
                remainder = rem_hi >= 0 ? mask_bits(src, rem_hi, 0) : 0;
            } else {
                remainder = src;
            }
            e.f_x = remainder << p.zero_pad;
            e.f_v &= (1u << 11) - 1;
            e.f_x &= (uint64_t(1) << 54) - 1;
            return e;
        }
    }

    switch (cnt) {
        case 0x3e6:  // {1'b1, src, 1'b0}
            e.f_v = 0;
            e.f_x = ((uint64_t(1) << 53) | (src << 1)) & ((uint64_t(1) << 54) - 1);
            break;
        case 0x3e5:  // {2'b1, src}
            e.f_v = 0;
            e.f_x = ((uint64_t(1) << 52) | src) & ((uint64_t(1) << 54) - 1);
            break;
        default:
            e.f_v = 0;
            e.f_x = uint64_t(1) << 51;  // {3'b1, 51'b0}
            break;
    }
    return e;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fcnvt_dtoh_sh dut;

    std::vector<Scenario> scenarios;
    for (int cnt = 0x3f0; cnt >= 0x3e5; --cnt) {
        scenarios.push_back({static_cast<uint16_t>(cnt),
                             (0x123456789ABCLL ^ static_cast<uint16_t>(cnt)) &
                                 ((uint64_t(1) << 52) - 1)});
    }
    scenarios.push_back({0x0000, 0x0ULL});  // default path

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    for (const auto& sc : scenarios) {
        dut.dtos_sh_cnt = sc.cnt;
        dut.dtos_sh_src = sc.src;
        dut.eval();

        Expected e = compute_expected(sc.cnt, sc.src);
        if (!expect(dut.dtos_sh_f_v == e.f_v,
                    "cnt 0x" + std::to_string(sc.cnt) + " f_v mismatch")) {
            return 1;
        }
        if (!expect((dut.dtos_sh_f_x & ((uint64_t(1) << 54) - 1)) == e.f_x,
                    "cnt 0x" + std::to_string(sc.cnt) + " f_x mismatch")) {
            return 1;
        }
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case012/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
