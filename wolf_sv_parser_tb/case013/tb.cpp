#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fcnvt_dtos_sh.h"

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
    uint32_t f_v;
    uint64_t f_x;
};

Expected compute_expected(uint16_t cnt, uint64_t raw_src) {
    uint64_t src = raw_src & ((uint64_t(1) << 52) - 1);
    Expected e{};

    if (cnt >= 875 && cnt <= 896) {
        int const_width = 898 - cnt;          // 2..23
        int slice_width = 24 - const_width;   // 22..1
        uint32_t prefix = 1u << (24 - const_width);
        uint32_t slice = static_cast<uint32_t>(mask_bits(src, 51, 52 - slice_width));
        e.f_v = prefix | slice;

        int rem_hi = 52 - slice_width - 1;  // bits below slice
        uint64_t remainder = rem_hi >= 0 ? mask_bits(src, rem_hi, 0) : 0;
        int zero_pad = 26 - const_width;
        e.f_x = remainder << zero_pad;
    } else if (cnt == 874) {
        e.f_v = 1u;
        e.f_x = src << 2;
    } else if (cnt == 873) {
        e.f_v = 0;
        e.f_x = ((uint64_t(1) << 53) | (src << 1));
    } else if (cnt == 872) {
        e.f_v = 0;
        e.f_x = ((uint64_t(1) << 52) | src);
    } else {
        e.f_v = 0;
        e.f_x = uint64_t(1) << 51;  // default {3'b1,51'b0}
    }

    e.f_v &= (1u << 24) - 1;
    e.f_x &= (uint64_t(1) << 54) - 1;
    return e;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fcnvt_dtos_sh dut;

    std::vector<Scenario> scenarios;
    for (int cnt = 896; cnt >= 872; --cnt) {
        scenarios.push_back({static_cast<uint16_t>(cnt),
                             (0xABCDEF123456ULL ^ static_cast<uint16_t>(cnt)) &
                                 ((uint64_t(1) << 52) - 1)});
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
        cov_out = "build/case013/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
