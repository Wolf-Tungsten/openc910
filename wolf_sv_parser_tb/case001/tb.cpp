#include <verilated.h>
#include <verilated_cov.h>
#include "Vct_fadd_close_s0_d.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fadd_close_s0_d dut;

    const vluint64_t mask53 = (1ULL << 53) - 1ULL;

    // A small suite of deterministic patterns to cover positive/negative/equal paths
    // and varied leading-one positions to exercise ff1_pred case arms.
    std::vector<std::pair<vluint64_t, vluint64_t>> tests = {
        {0, 0},
        {1, 0},
        {0, 1},
        {mask53, 1},
        {0x1555555555555ULL, 0x0AAAAAAAAAAAAULL},   // alternating bits
        {0x1ABCDEFULL, 0x0123456789ULL},
        {0x4000000000000ULL, 0x2000000000000ULL},   // high bits spaced
        {0x1ULL << 10, 0},
        {0, 0x1ULL << 20},
        {0x1ULL << 30, 0x1ULL << 5},
        {mask53, mask53},                           // equal non-zero
        {0x7FFFFFFFFFFFFULL, 0x3FFFFFFFFFFFFULL}
    };

    // Sweep single-bit patterns to touch many ff1_pred positions.
    for (int i = 0; i < 53; i += 4) {
        tests.push_back({1ULL << i, 0});
        tests.push_back({0, 1ULL << i});
    }

    for (size_t idx = 0; idx < tests.size(); ++idx) {
        vluint64_t op0 = tests[idx].first & mask53;
        vluint64_t op1 = tests[idx].second & mask53;
        dut.close_adder0 = op0;
        dut.close_adder1 = op1;
        dut.eval();

        std::cout << "case " << idx
                  << " close_adder0 0x" << std::hex << op0
                  << " close_adder1 0x" << op1 << std::dec
                  << " eq " << int(dut.close_eq)
                  << " op_chg " << int(dut.close_op_chg)
                  << " ff1_pred " << int(dut.ff1_pred)
                  << "\n";
    }

    const char* cov_env = std::getenv("COV_OUT");
    std::string cov_path = cov_env ? cov_env : "build/case001/coverage.dat";
    VerilatedCov::write(cov_path.c_str());
    std::cout << "Coverage written to " << cov_path << "\n";
    return 0;
}
