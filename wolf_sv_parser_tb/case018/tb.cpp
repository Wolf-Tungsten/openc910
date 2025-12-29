#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fcnvt_stod_sh.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Scenario {
    uint32_t src;
};

struct Expected {
    uint16_t cnt;
    uint32_t f_v;
};

Expected compute_expected(uint32_t src) {
    Expected e{};
    if (src == 0) {
        return e;
    }
    int msb = -1;
    for (int i = 22; i >= 0; --i) {
        if (src & (1u << i)) {
            msb = i;
            break;
        }
    }
    int shift = 23 - msb;
    e.f_v = ((src & ((1u << (msb + 1)) - 1)) << shift) & ((1u << 24) - 1);
    e.cnt = static_cast<uint16_t>(0xF6B + msb);
    return e;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fcnvt_stod_sh dut;

    std::vector<Scenario> scenarios;
    scenarios.push_back({0});
    for (int i = 0; i < 23; ++i) {
        scenarios.push_back({static_cast<uint32_t>(1u << i)});
    }
    scenarios.push_back({0x7FFFFF});
    scenarios.push_back({0x00412345});

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    for (const auto& sc : scenarios) {
        dut.stod_sh_src = sc.src;
        dut.eval();

        Expected e = compute_expected(sc.src);
        if (!expect(dut.stod_sh_cnt == e.cnt, "cnt mismatch")) {
            return 1;
        }
        if (!expect(dut.stod_sh_f_v == e.f_v, "f_v mismatch")) {
            return 1;
        }
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case018/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
