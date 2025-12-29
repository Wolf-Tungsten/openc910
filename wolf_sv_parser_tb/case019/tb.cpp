#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fcnvt_stoh_sh.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

uint32_t mask_bits(uint32_t value, int hi, int lo) {
    if (hi < lo) {
        return 0;
    }
    int width = hi - lo + 1;
    return (value >> lo) & ((1u << width) - 1);
}

struct Scenario {
    uint16_t cnt;
    uint32_t src;
};

struct Expected {
    uint16_t f_v;
    uint32_t f_x;
};

Expected compute_expected(uint16_t cnt, uint32_t raw_src) {
    uint32_t src = raw_src & ((1u << 23) - 1);
    Expected e{};

    struct Pattern {
        uint16_t cnt;
        int const_width;
        int slice_hi;
        int slice_lo;
        int zero_pad;
    };
    static const std::vector<Pattern> patterns = {
        {0x70, 2, 22, 14, 11}, {0x6F, 3, 22, 15, 10}, {0x6E, 4, 22, 16, 9},
        {0x6D, 5, 22, 17, 8}, {0x6C, 6, 22, 18, 7}, {0x6B, 7, 22, 19, 6},
        {0x6A, 8, 22, 20, 5}, {0x69, 9, 22, 21, 4}, {0x68, 10, 22, 22, 3},
        {0x67, 11, -1, -1, 2},
    };

    for (const auto& p : patterns) {
        if (cnt == p.cnt) {
            uint16_t prefix = static_cast<uint16_t>(1u << (11 - p.const_width));
            uint16_t slice = 0;
            if (p.slice_hi >= 0) {
                slice = static_cast<uint16_t>(mask_bits(src, p.slice_hi, p.slice_lo));
            }
            e.f_v = prefix | slice;

            uint32_t remainder = 0;
            if (p.slice_hi >= 0) {
                int rem_hi = p.slice_lo - 1;
                remainder = rem_hi >= 0 ? mask_bits(src, rem_hi, 0) : 0;
            } else {
                remainder = src;
            }
            e.f_x = remainder << p.zero_pad;
            e.f_v &= (1u << 11) - 1;
            e.f_x &= (1u << 25) - 1;
            return e;
        }
    }

    switch (cnt) {
        case 0x66:
            e.f_v = 0;
            e.f_x = ((1u << 24) | (src << 1)) & ((1u << 25) - 1);
            break;
        case 0x65:
            e.f_v = 0;
            e.f_x = ((1u << 23) | src) & ((1u << 25) - 1);
            break;
        default:
            e.f_v = 0;
            e.f_x = 1u << 22;  // default {3'b1,22'b0}
            break;
    }
    return e;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fcnvt_stoh_sh dut;

    std::vector<Scenario> scenarios;
    for (int cnt = 0x70; cnt >= 0x65; --cnt) {
        scenarios.push_back({static_cast<uint16_t>(cnt), (0xABCDEu ^ static_cast<uint16_t>(cnt)) & ((1u << 23) - 1)});
    }
    scenarios.push_back({0, 0});  // default path

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    for (const auto& sc : scenarios) {
        dut.stoh_sh_cnt = sc.cnt;
        dut.stoh_sh_src = sc.src;
        dut.eval();

        Expected e = compute_expected(sc.cnt, sc.src);
        if (!expect(dut.stoh_sh_f_v == e.f_v, "f_v mismatch")) {
            return 1;
        }
        if (!expect((dut.stoh_sh_f_x & ((1u << 25) - 1)) == e.f_x, "f_x mismatch")) {
            return 1;
        }
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case019/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
