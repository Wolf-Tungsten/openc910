#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fspu_single.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

uint32_t canonical_single(uint64_t raw, bool scalar) {
    const bool cnan = scalar && ((raw >> 32) != 0xFFFFFFFFULL);
    return cnan ? 0x7fc00000u : static_cast<uint32_t>(raw & 0xFFFFFFFFu);
}

uint32_t canonical_mtvr(uint64_t raw, bool check_nan) {
    const bool cnan = check_nan && ((raw >> 32) != 0xFFFFFFFFULL);
    return cnan ? 0x7fc00000u : static_cast<uint32_t>(raw & 0xFFFFFFFFu);
}

uint32_t fclass_single_bits(uint32_t v) {
    const bool sign = (v >> 31) & 1u;
    const uint32_t exp = (v >> 23) & 0xFFu;
    const uint32_t frac = v & 0x7FFFFFu;
    const bool exp_max = exp == 0xFFu;
    const bool exp_zero = exp == 0;
    const bool frac_zero = frac == 0;
    const bool frac_msb = (frac >> 22) & 1u;

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

    uint32_t res = 0;
    res |= static_cast<uint32_t>(neg_inf) << 0;
    res |= static_cast<uint32_t>(neg_nm) << 1;
    res |= static_cast<uint32_t>(neg_dn) << 2;
    res |= static_cast<uint32_t>(neg_zero) << 3;
    res |= static_cast<uint32_t>(pos_zero) << 4;
    res |= static_cast<uint32_t>(pos_dn) << 5;
    res |= static_cast<uint32_t>(pos_nm) << 6;
    res |= static_cast<uint32_t>(pos_inf) << 7;
    res |= static_cast<uint32_t>(snan) << 8;
    res |= static_cast<uint32_t>(qnan) << 9;
    return res;
}

uint64_t build_result(uint32_t op0, uint32_t op1, uint32_t mtvr, bool fmvvf, bool fsgnj, bool fsgnjn, bool fsgnjx) {
    uint64_t res = 0;
    const uint64_t upper = 0xFFFFFFFFull;
    if (fmvvf) {
        res |= (upper << 32) | mtvr;
    }
    if (fsgnj) {
        const uint32_t sign = (op1 >> 31) & 1u;
        const uint32_t payload = op0 & 0x7FFFFFFFu;
        res |= (upper << 32) | (static_cast<uint32_t>((sign << 31) | payload));
    }
    if (fsgnjn) {
        const uint32_t sign = (~op1 >> 31) & 1u;
        const uint32_t payload = op0 & 0x7FFFFFFFu;
        res |= (upper << 32) | (static_cast<uint32_t>((sign << 31) | payload));
    }
    if (fsgnjx) {
        const uint32_t sign = ((op0 >> 31) ^ (op1 >> 31)) & 1u;
        const uint32_t payload = op0 & 0x7FFFFFFFu;
        res |= (upper << 32) | (static_cast<uint32_t>((sign << 31) | payload));
    }
    return res;
}

struct Scenario {
    const char* name;
    bool check_nan;
    bool scalar;
    bool fmvvf;
    bool fsgnj;
    bool fsgnjn;
    bool fsgnjx;
    uint64_t oper0;
    uint64_t oper1;
    uint64_t mtvr;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fspu_single dut;

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    std::vector<Scenario> scenarios = {
        {"fmvvf_nan", true, true, true, false, false, false, 0x12345678ULL, 0, 0x11223344ULL},
        {"fsgnj", false, true, false, true, false, false, 0x3f800000ULL, 0xbf800000ULL, 0},
        {"fsgnjn", false, false, false, false, true, false, 0xff800001ULL, 0x7f800001ULL, 0},
        {"fsgnjx", false, true, false, false, false, true, 0x7f7fffffULL, 0x00000001ULL, 0},
        {"class_neg_dn", false, false, false, false, false, false, 0x80000001ULL, 0, 0},
        {"class_neg_nm", false, false, false, false, false, false, 0x84000000ULL, 0, 0},
        {"class_neg_zero", false, false, false, false, false, false, 0x80000000ULL, 0, 0},
        {"class_pos_zero", false, false, false, false, false, false, 0x00000000ULL, 0, 0},
        {"class_pos_dn", false, false, false, false, false, false, 0x00000001ULL, 0, 0},
        {"class_pos_inf", false, false, false, false, false, false, 0x7f800000ULL, 0, 0},
        {"class_snan", false, false, false, false, false, false, 0x7f800001ULL, 0, 0},
    };

    for (const auto& sc : scenarios) {
        const uint32_t op0_c = canonical_single(sc.oper0, sc.scalar);
        const uint32_t op1_c = canonical_single(sc.oper1, sc.scalar);
        const uint32_t mtvr_c = canonical_mtvr(sc.mtvr, sc.check_nan);

        dut.check_nan = sc.check_nan;
        dut.ex1_scalar = sc.scalar;
        dut.ex1_op_fmvvf = sc.fmvvf;
        dut.ex1_op_fsgnj = sc.fsgnj;
        dut.ex1_op_fsgnjn = sc.fsgnjn;
        dut.ex1_op_fsgnjx = sc.fsgnjx;
        dut.ex1_oper0 = sc.oper0;
        dut.ex1_oper1 = sc.oper1;
        dut.mtvr_src0 = sc.mtvr;
        dut.eval();

        const uint64_t expected_result = build_result(op0_c, op1_c, mtvr_c, sc.fmvvf, sc.fsgnj, sc.fsgnjn, sc.fsgnjx);
        const uint32_t expected_class = fclass_single_bits(op0_c);
        const uint64_t expected_fmfvr = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(sc.oper0 & 0xFFFFFFFFu)));

        if (!expect(dut.ex1_result == expected_result, std::string(sc.name) + " ex1_result mismatch")) return 1;
        if (!expect(dut.result_fclass == expected_class, std::string(sc.name) + " fclass mismatch")) return 1;
        if (!expect(dut.result_fmfvr == expected_fmfvr, std::string(sc.name) + " fmfvr mismatch")) return 1;
    }

    // Classification sweeps.
    for (uint64_t i = 0; i < 256; ++i) {
        const uint64_t op = (i << 23) ^ (i << 8);
        const uint32_t op_c = canonical_single(op, (i & 1) != 0);
        dut.ex1_scalar = (i & 1) != 0;
        dut.check_nan = (i & 2) != 0;
        dut.ex1_op_fmvvf = false;
        dut.ex1_op_fsgnj = true;
        dut.ex1_op_fsgnjn = false;
        dut.ex1_op_fsgnjx = false;
        dut.ex1_oper0 = op;
        dut.ex1_oper1 = ~op;
        dut.mtvr_src0 = op ^ 0x5555AAAAu;
        dut.eval();
        if (!expect(dut.result_fclass == fclass_single_bits(op_c), "sweep fclass mismatch")) return 1;
    }

    // Toggle bits broadly to improve toggle coverage.
    const uint64_t all_ones = ~0ULL;
    dut.ex1_scalar = 1;
    dut.check_nan = 1;
    dut.ex1_oper0 = all_ones;
    dut.ex1_oper1 = all_ones;
    dut.mtvr_src0 = all_ones;
    dut.ex1_op_fmvvf = 0;
    dut.ex1_op_fsgnj = 0;
    dut.ex1_op_fsgnjn = 0;
    dut.ex1_op_fsgnjx = 0;
    dut.eval();
    for (int i = 0; i < 64; ++i) {
        dut.ex1_oper0 = all_ones ^ (1ULL << i);
        dut.ex1_oper1 = 1ULL << i;
        dut.mtvr_src0 = 1ULL << i;
        dut.eval();
    }
    dut.ex1_scalar = 0;
    dut.check_nan = 0;
    dut.ex1_oper0 = 0;
    dut.ex1_oper1 = 0;
    dut.mtvr_src0 = 0;
    dut.eval();

    // Exercise ex1_result mux paths explicitly.
    dut.ex1_oper0 = 0x7f800000ULL;
    dut.ex1_oper1 = 0x80000000ULL;
    dut.mtvr_src0 = 0xAAAAAAAAULL;
    dut.ex1_op_fmvvf = 1;
    dut.eval();
    dut.ex1_op_fmvvf = 0;
    dut.ex1_op_fsgnj = 1;
    dut.eval();
    dut.ex1_op_fsgnj = 0;
    dut.ex1_op_fsgnjn = 1;
    dut.eval();
    dut.ex1_op_fsgnjn = 0;
    dut.ex1_op_fsgnjx = 1;
    dut.eval();
    dut.ex1_op_fsgnjx = 0;
    dut.eval();

    // Randomized sweeps per operation.
    uint64_t lcg = 7;
    auto next = [&]() {
        lcg = lcg * 1103515245ULL + 12345ULL;
        return lcg;
    };
    for (int mode = 0; mode < 4; ++mode) {
        dut.ex1_op_fmvvf = (mode == 0);
        dut.ex1_op_fsgnj = (mode == 1);
        dut.ex1_op_fsgnjn = (mode == 2);
        dut.ex1_op_fsgnjx = (mode == 3);
        dut.ex1_scalar = (mode & 1) != 0;
        dut.check_nan = (mode & 2) != 0;
        for (int j = 0; j < 256; ++j) {
            uint64_t v = next();
            dut.ex1_oper0 = v;
            dut.ex1_oper1 = ~v;
            dut.mtvr_src0 = v ^ 0x13572468u;
            dut.eval();
        }
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case024/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
