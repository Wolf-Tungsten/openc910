#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fadd_close_s1_d.h"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fadd_close_s1_d dut;

    const vluint64_t mask54 = (1ULL << 54) - 1ULL;

    auto drive = [&](vluint64_t a, vluint64_t b, bool ex_double, bool ex_single) {
        dut.close_adder0 = a & mask54;
        dut.close_adder1 = b & mask54;
        dut.ex1_double = ex_double;
        dut.ex1_single = ex_single;
        dut.eval();
    };

    // Toggle the ex1 flags through all basic states before functional checks.
    drive(0, 0, true, false);
    drive(0, 0, false, true);
    drive(0, 0, false, false);

    std::array<bool, 54> ff1_hits{};

    // Sweep a one-hot across close_adder0 to cover every ff1_pred branch.
    for (int bit = 0; bit < 54; ++bit) {
        vluint64_t val = 1ULL << bit;
        drive(val, 0, true, false);

        unsigned expected_pred = 53 - bit;
        if (dut.ff1_pred != expected_pred || dut.ff1_pred_onehot != val || dut.close_sum != val) {
            std::cerr << "ff1 mismatch at bit " << bit
                      << " ff1_pred=" << dut.ff1_pred
                      << " ff1_pred_onehot=0x" << std::hex << dut.ff1_pred_onehot
                      << " close_sum=0x" << dut.close_sum
                      << " expected_pred=" << std::dec << expected_pred
                      << " expected_onehot=0x" << std::hex << val << std::dec << "\n";
            return 1;
        }
        ff1_hits[dut.ff1_pred] = true;
    }

    // Drive one-hots on close_adder1 to exercise the z-path and negative sums.
    for (int bit = 0; bit < 54; ++bit) {
        vluint64_t val = 1ULL << bit;
        drive(0, val, false, true);
        ff1_hits[std::min<unsigned>(dut.ff1_pred, ff1_hits.size() - 1)] = true;
    }

    // A handful of mixed patterns to toggle close_sum sign and shared logic.
    std::vector<std::pair<vluint64_t, vluint64_t>> patterns = {
        {mask54, 0},
        {0, mask54},
        {mask54, mask54},
        {(1ULL << 53) | 0x12345ULL, (1ULL << 52) | 0x22222ULL},
        {0x155555555555ULL & mask54, 0x0AAAAAAAAAAAULL & mask54},
        {1, 0},
        {0, 1},
        {(1ULL << 20) | 1ULL, (1ULL << 19)}
    };

    bool ex_toggle = false;
    for (const auto& p : patterns) {
        drive(p.first, p.second, ex_toggle, !ex_toggle);
        ex_toggle = !ex_toggle;
        if (dut.ff1_pred < ff1_hits.size()) {
            ff1_hits[dut.ff1_pred] = true;
        }
    }

    size_t unique_preds = 0;
    for (bool hit : ff1_hits) {
        unique_preds += hit ? 1 : 0;
    }

    const char* cov_env = std::getenv("COV_OUT");
    std::string cov_path = cov_env ? cov_env : "build/case003/coverage.dat";
    VerilatedCov::write(cov_path.c_str());

    std::cout << "Unique ff1_pred values observed: " << unique_preds << "/54\n";
    std::cout << "Final close_sum=0x" << std::hex << dut.close_sum << std::dec
              << " close_op_chg=" << int(dut.close_op_chg)
              << " ex1_double=" << int(dut.ex1_double)
              << " ex1_single=" << int(dut.ex1_single) << "\n";
    std::cout << "Coverage written to " << cov_path << "\n";
    return 0;
}
