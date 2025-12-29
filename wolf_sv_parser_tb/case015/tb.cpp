#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fcnvt_htos_sh.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Scenario {
    uint16_t src;
};

struct Expected {
    uint16_t cnt;
    uint16_t f_v;
};

Expected compute_expected(uint16_t src) {
    Expected e{};
    if (src == 0) {
        return e;
    }
    int msb = -1;
    for (int i = 9; i >= 0; --i) {
        if (src & (1u << i)) {
            msb = i;
            break;
        }
    }
    int shift = 10 - msb;
    e.f_v = static_cast<uint16_t>((src & ((1u << (msb + 1)) - 1)) << shift);
    e.f_v &= (1u << 11) - 1;
    e.cnt = static_cast<uint16_t>(0x28 + msb);
    return e;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fcnvt_htos_sh dut;

    std::vector<Scenario> scenarios;
    scenarios.push_back({0});
    for (int i = 0; i < 10; ++i) {
        scenarios.push_back({static_cast<uint16_t>(1u << i)});
    }
    scenarios.push_back({0x3FF});
    scenarios.push_back({0x2A3});

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    for (const auto& sc : scenarios) {
        dut.htos_sh_src = sc.src;
        dut.eval();

        Expected e = compute_expected(sc.src);
        if (!expect(dut.htos_sh_cnt == e.cnt,
                    "src 0x" + std::to_string(sc.src) + " cnt mismatch")) {
            return 1;
        }
        if (!expect(dut.htos_sh_f_v == e.f_v,
                    "src 0x" + std::to_string(sc.src) + " f_v mismatch")) {
            return 1;
        }
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case015/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
