#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fadd_close_s0_h.h"

#include <array>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fadd_close_s0_h dut;

    constexpr unsigned kWidth = 11;
    constexpr unsigned kRange = 1u << kWidth; // 2048

    std::array<bool, 2> eq_seen{};          // close_eq true/false
    std::array<bool, 2> op_chg_seen{};      // close_op_chg true/false
    std::array<bool, 16> pred_seen{};       // ff1_pred values hit
    std::array<bool, 1u << kWidth> onehot_seen{}; // ff1_pred_onehot values hit

    // Force deterministic low start, then toggle each input bit both ways to hit
    // per-bit toggle coverage points before the exhaustive sweep.
    dut.close_adder0 = 0;
    dut.close_adder1 = 0;
    dut.eval();
    for (unsigned bit = 0; bit < kWidth; ++bit) {
        vluint64_t mask = 1u << bit;
        dut.close_adder0 = mask;
        dut.eval();
        dut.close_adder0 = 0;
        dut.eval();

        dut.close_adder1 = mask;
        dut.eval();
        dut.close_adder1 = 0;
        dut.eval();
    }

    for (unsigned a = 0; a < kRange; ++a) {
        for (unsigned b = 0; b < kRange; ++b) {
            dut.close_adder0 = a;
            dut.close_adder1 = b;
            dut.eval();

            eq_seen[dut.close_eq] = true;
            op_chg_seen[dut.close_op_chg] = true;
            pred_seen[dut.ff1_pred & 0xF] = true;
            onehot_seen[dut.ff1_pred_onehot & (onehot_seen.size() - 1)] = true;
        }
    }

    const char* cov_env = std::getenv("COV_OUT");
    std::string cov_path = cov_env ? cov_env : "build/case002/coverage.dat";
    VerilatedCov::write(cov_path.c_str());

    auto count_hits = [](const auto& container) {
        return std::count(container.begin(), container.end(), true);
    };

    std::cout << "close_eq states hit: " << count_hits(eq_seen)
              << "/2, close_op_chg states hit: " << count_hits(op_chg_seen)
              << "/2, ff1_pred unique values: " << count_hits(pred_seen)
              << ", ff1_pred_onehot unique values: " << count_hits(onehot_seen)
              << "\n";
    std::cout << "Coverage written to " << cov_path << "\n";
    return 0;
}
