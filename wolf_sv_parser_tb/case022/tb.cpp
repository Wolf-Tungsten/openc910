#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fspu_double.h"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

uint64_t make_fclass_double(uint64_t v) {
    const bool sign = (v >> 63) & 1;
    const uint16_t exp = static_cast<uint16_t>((v >> 52) & 0x7FF);
    const uint64_t frac = v & ((1ULL << 52) - 1);
    const bool exp_max = exp == 0x7FF;
    const bool exp_zero = exp == 0;
    const bool frac_zero = frac == 0;
    const bool frac_msb = (frac >> 51) & 1;

    const bool neg_inf = sign && exp_max && frac_zero;
    const bool neg_nm = sign && !exp_max && !exp_zero;
    const bool neg_dn = sign && exp_zero && !frac_zero;
    const bool neg_zero = sign && exp_zero && frac_zero;
    const bool pos_zero = !sign && exp_zero && frac_zero;
    const bool pos_dn = !sign && exp_zero && !frac_zero;
    const bool pos_nm = !sign && !exp_max && !exp_zero;
    const bool pos_inf = !sign && exp_max && frac_zero;
    const bool snan = exp_max && !frac_zero && !frac_msb;
    const bool qnan = exp_max && frac_msb;

    uint64_t bits = 0;
    bits |= static_cast<uint64_t>(neg_inf) << 0;
    bits |= static_cast<uint64_t>(neg_nm) << 1;
    bits |= static_cast<uint64_t>(neg_dn) << 2;
    bits |= static_cast<uint64_t>(neg_zero) << 3;
    bits |= static_cast<uint64_t>(pos_zero) << 4;
    bits |= static_cast<uint64_t>(pos_dn) << 5;
    bits |= static_cast<uint64_t>(pos_nm) << 6;
    bits |= static_cast<uint64_t>(pos_inf) << 7;
    bits |= static_cast<uint64_t>(snan) << 8;
    bits |= static_cast<uint64_t>(qnan) << 9;
    return bits;
}

uint64_t apply_fsgnj_ops(uint64_t oper0, uint64_t oper1, bool sgnj, bool sgnjn, bool sgnjx) {
    uint64_t payload = oper0 & ((1ULL << 63) - 1);
    uint64_t sign = 0;
    if (sgnj) {
        sign = (oper1 >> 63) & 1ULL;
    } else if (sgnjn) {
        sign = (~oper1 >> 63) & 1ULL;
    } else if (sgnjx) {
        sign = ((oper0 >> 63) ^ (oper1 >> 63)) & 1ULL;
    }
    return (sign << 63) | payload;
}

struct Scenario {
    const char* name;
    bool op_fmvvf;
    bool op_fsgnj;
    bool op_fsgnjn;
    bool op_fsgnjx;
    uint64_t oper0;
    uint64_t oper1;
    uint64_t mtvr;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fspu_double dut;

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };
    auto expect_eq = [&](uint64_t actual, uint64_t expected, const std::string& msg) {
        if (actual != expected) {
            std::cerr << msg << " expected=0x" << std::hex << expected << " got=0x" << actual << std::dec << "\n";
            return false;
        }
        return true;
    };

    std::vector<Scenario> scenarios = {
        {"fmvvf_move", true, false, false, false, 0x0123456789ABCDEFULL, 0x0, 0xDEADBEEFCAFEBABEULL},
        {"fsgnj_keep", false, true, false, false, 0x7FF0000000000001ULL, 0x8000000000000000ULL, 0},
        {"fsgnjn_flip", false, false, true, false, 0x3FF0000000000000ULL, 0x3FF8000000000000ULL, 0},
        {"fsgnjx_xor", false, false, false, true, 0xBFEFFFFFFFFFFFFFULL, 0x7FF8000000000000ULL, 0},
        {"class_pos_denorm", false, false, false, false, 0x0000000000000001ULL, 0, 0},
        {"class_neg_denorm", false, false, false, false, 0x8000000000000001ULL, 0, 0},
    };

    // Add a mix of corner classifications to exercise the decode logic.
    std::array<uint64_t, 6> klass_inputs = {
        0x0000000000000000ULL,  // +0
        0x8000000000000000ULL,  // -0
        0x7FF0000000000000ULL,  // +inf
        0xFFF0000000000000ULL,  // -inf
        0x7FF0000000000001ULL,  // snan
        0x7FF8000000000000ULL,  // qnan
    };
    for (uint64_t v : klass_inputs) {
        scenarios.push_back({"class_only", false, false, false, false, v, ~v, 0});
    }

    for (const auto& sc : scenarios) {
        dut.ex1_op_fmvvf = sc.op_fmvvf;
        dut.ex1_op_fsgnj = sc.op_fsgnj;
        dut.ex1_op_fsgnjn = sc.op_fsgnjn;
        dut.ex1_op_fsgnjx = sc.op_fsgnjx;
        dut.ex1_oper0 = sc.oper0;
        dut.ex1_oper1 = sc.oper1;
        dut.mtvr_src0 = sc.mtvr;
        dut.eval();

        const uint64_t class_expected = make_fclass_double(sc.oper0);
        const uint64_t fsgn_expected = apply_fsgnj_ops(sc.oper0, sc.oper1, sc.op_fsgnj, sc.op_fsgnjn, sc.op_fsgnjx);
        const uint64_t ex1_result_expected = sc.op_fmvvf ? sc.mtvr : (sc.op_fsgnj || sc.op_fsgnjn || sc.op_fsgnjx ? fsgn_expected : 0);

        if (!expect_eq(dut.result_fclass, class_expected, std::string(sc.name) + " fclass mismatch")) return 1;
        if (!expect_eq(dut.result_fmfvr, sc.oper0, std::string(sc.name) + " fmfvr mismatch")) return 1;
        if (!expect_eq(dut.ex1_result, ex1_result_expected, std::string(sc.name) + " ex1_result mismatch")) return 1;
    }

    // Toggle every bit through the fmvvf path to exercise result formatting.
    dut.ex1_op_fmvvf = 1;
    dut.ex1_op_fsgnj = 0;
    dut.ex1_op_fsgnjn = 0;
    dut.ex1_op_fsgnjx = 0;
    dut.ex1_oper0 = 0x3FF0000000000000ULL;  // 1.0 -> pos_nm class
    dut.ex1_oper1 = 0;
    const uint64_t class_base = make_fclass_double(dut.ex1_oper0);
    for (int i = 0; i < 64; ++i) {
        dut.mtvr_src0 = 1ULL << i;
        dut.eval();
        if (!expect_eq(dut.ex1_result, dut.mtvr_src0, "fmvvf bit sweep result mismatch")) return 1;
        if (!expect_eq(dut.result_fclass, class_base, "fmvvf bit sweep fclass mismatch")) return 1;
    }

    // Randomized sweep to toggle more bits for coverage.
    uint64_t lcg = 1;
    auto next = [&]() {
        lcg = lcg * 6364136223846793005ULL + 1;
        return lcg;
    };
    for (int i = 0; i < 512; ++i) {
        const uint64_t oper0 = next();
        const uint64_t oper1 = next();
        const uint32_t op_idx = static_cast<uint32_t>(next() & 0x3);
        const bool fmvvf = op_idx == 0;
        const bool fsgnj = op_idx == 1;
        const bool fsgnjn = op_idx == 2;
        const bool fsgnjx = op_idx == 3;
        dut.ex1_oper0 = oper0;
        dut.ex1_oper1 = oper1;
        dut.mtvr_src0 = next();
        dut.ex1_op_fmvvf = fmvvf;
        dut.ex1_op_fsgnj = fsgnj;
        dut.ex1_op_fsgnjn = fsgnjn;
        dut.ex1_op_fsgnjx = fsgnjx;
        dut.eval();
        // Basic sanity: classification always matches helper.
        if (!expect(dut.result_fclass == make_fclass_double(oper0), "random fclass mismatch")) return 1;
        if (!expect(dut.result_fmfvr == oper0, "random fmfvr mismatch")) return 1;
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case022/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
