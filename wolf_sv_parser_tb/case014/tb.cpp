#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fcnvt_ftoi_sh.h"

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
    uint8_t cnt;
    uint64_t src;
};

struct Expected {
    uint64_t v;
    uint64_t x;
};

Expected compute_expected(uint8_t cnt, uint64_t raw_src) {
    uint64_t src = raw_src & ((uint64_t(1) << 53) - 1);
    Expected e{};

    if (cnt == 0x7f) {
        e.v = 0;
        e.x = (src << 1) & ((uint64_t(1) << 54) - 1);
        return e;
    }

    if (cnt <= 52) {
        int slice_width = cnt + 1;
        e.v = mask_bits(src, 52, 52 - slice_width + 1);
        uint64_t remainder_mask = (cnt == 52) ? 0 : ((uint64_t(1) << (52 - cnt)) - 1);
        uint64_t remainder = src & remainder_mask;
        e.x = remainder << (cnt + 2);
    } else if (cnt <= 63) {
        int shift = cnt - 52;
        e.v = src << shift;
        e.x = 0;
    }
    return e;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fcnvt_ftoi_sh dut;

    std::vector<Scenario> scenarios;
    for (uint8_t cnt = 0; cnt <= 63; ++cnt) {
        scenarios.push_back({cnt, (0x13579BDF2468ULL ^ cnt) & ((uint64_t(1) << 53) - 1)});
    }
    scenarios.push_back({0x7f, (uint64_t(1) << 52) - 1});

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    for (const auto& sc : scenarios) {
        dut.fsh_cnt = sc.cnt;
        dut.fsh_src = sc.src;
        dut.eval();

        Expected e = compute_expected(sc.cnt, sc.src);
        if (!expect(dut.fsh_i_v_nm == e.v,
                    "cnt " + std::to_string(sc.cnt) + " fsh_i_v_nm mismatch")) {
            return 1;
        }
        if (!expect(dut.fsh_i_x_nm == e.x,
                    "cnt " + std::to_string(sc.cnt) + " fsh_i_x_nm mismatch")) {
            return 1;
        }
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case014/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
