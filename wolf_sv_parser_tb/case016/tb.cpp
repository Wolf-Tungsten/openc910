#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fcnvt_itof_sh.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

int leading_one(uint64_t value) {
    if (value == 0) {
        return -1;
    }
#if defined(__GNUC__)
    return 63 - __builtin_clzll(value);
#else
    for (int i = 63; i >= 0; --i) {
        if (value & (uint64_t(1) << i)) {
            return i;
        }
    }
    return -1;
#endif
}

struct Scenario {
    uint64_t src;
};

struct Expected {
    uint8_t cnt;
    uint8_t cnt_p1;
    uint32_t f_v;
    uint64_t f_x;
    bool c_in;
};

Expected compute_expected(uint64_t src) {
    Expected e{};
    int msb = leading_one(src);
    if (msb < 0) {
        return e;
    }

    e.cnt = static_cast<uint8_t>(msb);
    e.cnt_p1 = static_cast<uint8_t>((msb == 63) ? 0 : msb + 1);

    if (msb >= 23) {
        int shift_down = msb - 23;
        e.f_v = static_cast<uint32_t>((src >> shift_down) & ((uint64_t(1) << 24) - 1));
        int rem_bits = msb - 23;
        if (rem_bits > 0) {
            uint64_t remainder_mask = (uint64_t(1) << rem_bits) - 1;
            uint64_t remainder = src & remainder_mask;
            int zero_pad = 40 - rem_bits;
            e.f_x = remainder << zero_pad;
        }
    } else {
        int shift_up = 23 - msb;
        e.f_v = static_cast<uint32_t>((src & ((uint64_t(1) << (msb + 1)) - 1)) << shift_up);
        e.f_x = 0;
    }

    e.f_v &= (1u << 24) - 1;
    e.f_x &= (uint64_t(1) << 40) - 1;

    uint64_t mask = (msb == 63) ? ~uint64_t(0) : ((uint64_t(1) << (msb + 1)) - 1);
    e.c_in = (src & mask) == mask;
    return e;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fcnvt_itof_sh dut;

    std::vector<Scenario> scenarios;
    scenarios.push_back({0});
    for (int i = 0; i < 64; ++i) {
        scenarios.push_back({uint64_t(1) << i});
    }
    scenarios.push_back({~uint64_t(0)});
    scenarios.push_back({0x8000000000000000ULL | 0x3FF});

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    for (const auto& sc : scenarios) {
        dut.ff1_sh_src = sc.src;
        dut.eval();

        Expected e = compute_expected(sc.src);
        if (!expect(dut.ff1_sh_cnt == e.cnt, "cnt mismatch")) {
            return 1;
        }
        if (!expect(dut.ff1_sh_cnt_p1 == e.cnt_p1, "cnt_p1 mismatch")) {
            return 1;
        }
        if (!expect(dut.ff1_sh_f_v == e.f_v, "f_v mismatch")) {
            return 1;
        }
        if (!expect(dut.ff1_sh_f_x == e.f_x, "f_x mismatch")) {
            return 1;
        }
        bool c_in = dut.ff1_sh_c_in;
        if (!expect(c_in == e.c_in, "c_in mismatch")) {
            return 1;
        }
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case016/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
