#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fspu_half.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

uint16_t canonical_half(uint64_t raw, bool scalar) {
    const bool cnan = scalar && ((raw >> 16) != 0xFFFFFFFFFFFFULL);
    return cnan ? static_cast<uint16_t>(0x7e00) : static_cast<uint16_t>(raw & 0xFFFF);
}

uint16_t canonical_mtvr(uint64_t raw, bool check_nan) {
    const bool cnan = check_nan && ((raw >> 16) != 0xFFFFFFFFFFFFULL);
    return cnan ? static_cast<uint16_t>(0x7e00) : static_cast<uint16_t>(raw & 0xFFFF);
}

uint16_t fclass_half_bits(uint16_t half) {
    const bool sign = (half >> 15) & 1;
    const uint16_t exp = (half >> 10) & 0x1F;
    const uint16_t frac = half & 0x3FF;
    const bool exp_max = exp == 0x1F;
    const bool exp_zero = exp == 0;
    const bool frac_zero = frac == 0;
    const bool frac_msb = (frac >> 9) & 1;

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

    uint16_t res = 0;
    res |= static_cast<uint16_t>(neg_inf) << 0;
    res |= static_cast<uint16_t>(neg_nm) << 1;
    res |= static_cast<uint16_t>(neg_dn) << 2;
    res |= static_cast<uint16_t>(neg_zero) << 3;
    res |= static_cast<uint16_t>(pos_zero) << 4;
    res |= static_cast<uint16_t>(pos_dn) << 5;
    res |= static_cast<uint16_t>(pos_nm) << 6;
    res |= static_cast<uint16_t>(pos_inf) << 7;
    res |= static_cast<uint16_t>(snan) << 8;
    res |= static_cast<uint16_t>(qnan) << 9;
    return res;
}

uint64_t build_result_from_op(uint16_t op0, uint16_t op1, uint16_t mtvr, bool fmvvf, bool fsgnj, bool fsgnjn, bool fsgnjx) {
    const uint64_t base = 0xFFFFFFFFFFFFULL;
    uint64_t res = 0;
    if (fmvvf) {
        res |= (base << 16) | mtvr;
    }
    if (fsgnj) {
        const uint16_t payload = op0 & 0x7FFF;
        const uint16_t sign = (op1 >> 15) & 1;
        res |= (base << 16) | (static_cast<uint16_t>((sign << 15) | payload));
    }
    if (fsgnjn) {
        const uint16_t payload = op0 & 0x7FFF;
        const uint16_t sign = (~op1 >> 15) & 1;
        res |= (base << 16) | (static_cast<uint16_t>((sign << 15) | payload));
    }
    if (fsgnjx) {
        const uint16_t payload = op0 & 0x7FFF;
        const uint16_t sign = ((op0 >> 15) ^ (op1 >> 15)) & 1;
        res |= (base << 16) | (static_cast<uint16_t>((sign << 15) | payload));
    }
    return res;
}

struct Scenario {
    const char* name;
    bool check_nan;
    bool scalar;
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

    Vct_fspu_half dut;

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    std::vector<Scenario> scenarios = {
        {"fmvvf_with_cnan", true, true, true, false, false, false, 0xFFFF'0000'0000'7E01ULL, 0, 0x0},
        {"fsgnj_basic", false, true, false, true, false, false, 0x3C00, 0xBC00, 0},
        {"fsgnjn_basic", false, false, false, false, true, false, 0xFC00, 0x0001, 0},
        {"fsgnjx_basic", false, true, false, false, false, true, 0x7BFF, 0xFFFF, 0},
        {"class_neg_dn", false, false, false, false, false, false, 0x0000'0000'0000'8001ULL, 0, 0},
        {"class_neg_nm", false, false, false, false, false, false, 0x0000'0000'0000'8400ULL, 0, 0},
        {"class_neg_zero", false, false, false, false, false, false, 0x0000'0000'0000'8000ULL, 0, 0},
        {"class_pos_inf", false, false, false, false, false, false, 0x0000'0000'0000'7C00ULL, 0, 0},
        {"class_snan", false, false, false, false, false, false, 0x0000'0000'0000'7C01ULL, 0, 0},
        {"class_pos_dn", false, false, false, false, false, false, 0x0000'0000'0000'0001ULL, 0, 0},
        {"class_pos_zero", false, false, false, false, false, false, 0x0, 0, 0},
        {"class_pos_nm", false, false, false, false, false, false, 0x0000'0000'0000'0400ULL, 0, 0},
        {"class_neg_inf", false, false, false, false, false, false, 0x0000'0000'0000'FC00ULL, 0, 0},
    };

    for (const auto& sc : scenarios) {
        const uint16_t op0_c = canonical_half(sc.oper0, sc.scalar);
        const uint16_t op1_c = canonical_half(sc.oper1, sc.scalar);
        const uint16_t mtvr_c = canonical_mtvr(sc.mtvr, sc.check_nan);

        dut.check_nan = sc.check_nan;
        dut.ex1_scalar = sc.scalar;
        dut.ex1_op_fmvvf = sc.op_fmvvf;
        dut.ex1_op_fsgnj = sc.op_fsgnj;
        dut.ex1_op_fsgnjn = sc.op_fsgnjn;
        dut.ex1_op_fsgnjx = sc.op_fsgnjx;
        dut.ex1_oper0 = sc.oper0;
        dut.ex1_oper1 = sc.oper1;
        dut.mtvr_src0 = sc.mtvr;
        dut.eval();

        const uint64_t expected_result = build_result_from_op(op0_c, op1_c, mtvr_c, sc.op_fmvvf, sc.op_fsgnj, sc.op_fsgnjn, sc.op_fsgnjx);
        const uint16_t expected_class = fclass_half_bits(op0_c);
        const uint64_t expected_fmfvr = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int16_t>(sc.oper0 & 0xFFFF)));

        if (!expect(dut.ex1_result == expected_result, std::string(sc.name) + " ex1_result mismatch")) return 1;
        if (!expect(dut.result_fclass == expected_class, std::string(sc.name) + " fclass mismatch")) return 1;
        if (!expect(dut.result_fmfvr == expected_fmfvr, std::string(sc.name) + " fmfvr mismatch")) return 1;
    }

    // Sweep a range of operands to toggle classification paths.
    for (uint64_t raw = 0; raw < 0x40; ++raw) {
        const uint64_t op = raw << 8;
        const uint16_t op_c = canonical_half(op, (raw & 1) != 0);
        dut.ex1_scalar = (raw & 1) != 0;
        dut.check_nan = (raw & 2) != 0;
        dut.ex1_op_fmvvf = false;
        dut.ex1_op_fsgnj = false;
        dut.ex1_op_fsgnjn = true;
        dut.ex1_op_fsgnjx = false;
        dut.ex1_oper0 = op;
        dut.ex1_oper1 = ~op;
        dut.mtvr_src0 = op ^ 0x1234;
        dut.eval();
        if (!expect(dut.result_fclass == fclass_half_bits(op_c), "sweep fclass mismatch")) return 1;
    }

    // Wide bit toggling to hit toggle coverage on inputs and CNAN paths.
    const uint64_t all_ones = ~0ULL;
    dut.ex1_scalar = 1;
    dut.check_nan = 1;
    dut.ex1_op_fmvvf = 0;
    dut.ex1_op_fsgnj = 0;
    dut.ex1_op_fsgnjn = 0;
    dut.ex1_op_fsgnjx = 0;
    dut.ex1_oper1 = all_ones;
    dut.mtvr_src0 = all_ones;
    dut.ex1_oper0 = all_ones;
    dut.eval();
    for (int i = 0; i < 64; ++i) {
        dut.ex1_oper0 = all_ones ^ (1ULL << i);
        dut.eval();
    }
    for (int i = 0; i < 64; ++i) {
        dut.ex1_oper1 = 1ULL << i;
        dut.eval();
    }
    for (int i = 0; i < 64; ++i) {
        dut.mtvr_src0 = 1ULL << i;
        dut.eval();
    }
    // Toggle with zero base to flip bits from 0->1.
    dut.ex1_scalar = 0;
    dut.check_nan = 0;
    dut.ex1_oper0 = 0;
    dut.ex1_oper1 = 0;
    dut.mtvr_src0 = 0;
    dut.eval();
    for (int i = 0; i < 64; ++i) {
        dut.ex1_oper0 = 1ULL << i;
        dut.ex1_oper1 = 1ULL << i;
        dut.mtvr_src0 = 1ULL << i;
        dut.eval();
    }
    // Force ex1_result to toggle between zero and active operation outputs.
    dut.ex1_oper0 = 0x0000'0000'0000'7E00ULL;
    dut.ex1_oper1 = 0x0000'0000'0000'8000ULL;
    dut.ex1_op_fmvvf = 0;
    dut.ex1_op_fsgnj = 0;
    dut.ex1_op_fsgnjn = 0;
    dut.ex1_op_fsgnjx = 0;
    dut.eval();  // ex1_result = 0
    dut.ex1_op_fmvvf = 1;
    dut.eval();  // ex1_result drives fmvvf path
    dut.ex1_op_fmvvf = 0;
    dut.ex1_op_fsgnj = 1;
    dut.eval();  // ex1_result drives fsgnj path
    dut.ex1_op_fsgnj = 0;
    dut.ex1_op_fsgnjn = 1;
    dut.eval();  // ex1_result drives fsgnjn path
    dut.ex1_op_fsgnjn = 0;
    dut.ex1_op_fsgnjx = 1;
    dut.eval();  // ex1_result drives fsgnjx path
    dut.ex1_op_fsgnjx = 0;
    dut.eval();  // back to zero
    // Drive all-ones then all-zeros on ex1_result and result_fmfvr outputs.
    dut.ex1_op_fmvvf = 0;
    dut.ex1_oper0 = 0;
    dut.mtvr_src0 = 0;
    dut.eval();  // zero outputs
    dut.ex1_op_fmvvf = 1;
    dut.mtvr_src0 = 0xFFFF;
    dut.eval();  // lower 16 bits ones, upper 48 ones
    dut.ex1_op_fmvvf = 0;
    dut.ex1_oper0 = 0xFFFF;
    dut.eval();  // result_fmfvr all ones
    dut.ex1_oper0 = 0;
    dut.eval();  // result_fmfvr zeros

    // Randomized sweeps per operation to toggle result buses.
    for (int op_mode = 0; op_mode < 4; ++op_mode) {
        dut.ex1_scalar = 0;
        dut.check_nan = 0;
        dut.ex1_op_fmvvf = (op_mode == 0);
        dut.ex1_op_fsgnj = (op_mode == 1);
        dut.ex1_op_fsgnjn = (op_mode == 2);
        dut.ex1_op_fsgnjx = (op_mode == 3);
        for (uint64_t val = 0; val < 256; ++val) {
            const uint64_t op0 = (val << 8) ^ (val << 1);
            const uint64_t op1 = ~op0;
            dut.ex1_oper0 = op0;
            dut.ex1_oper1 = op1;
            dut.mtvr_src0 = op0 ^ 0xAAAA;
            dut.eval();
        }
    }
    // Boost counts on specific classification cases and control toggles.
    for (int i = 0; i < 5; ++i) {
        // Toggle op flags repeatedly.
        dut.ex1_op_fsgnj = 1;
        dut.eval();
        dut.ex1_op_fsgnj = 0;
        dut.eval();
        dut.ex1_op_fsgnjx = 1;
        dut.eval();
        dut.ex1_op_fsgnjx = 0;
        dut.eval();

        // Hit inf/zero classification corners multiple times.
        dut.ex1_scalar = 0;
        dut.check_nan = 0;
        dut.ex1_oper0 = 0x0000'0000'0000'FC00ULL;  // neg_inf
        dut.ex1_oper1 = 0;
        dut.eval();
        dut.ex1_oper0 = 0x0000'0000'0000'8000ULL;  // neg_zero
        dut.eval();
        dut.ex1_oper0 = 0x0000'0000'0000'7C00ULL;  // pos_inf
        dut.eval();

        // Exercise op1_cnan path.
        dut.ex1_scalar = 1;
        dut.ex1_oper1 = 0x0;  // upper bits not all ones -> cnan asserted
        dut.eval();
        dut.ex1_scalar = 0;
    }
    // Explicitly toggle each classification flag on then off.
    auto drive_half = [&](uint16_t v) {
        dut.ex1_scalar = 0;
        dut.check_nan = 0;
        dut.ex1_oper0 = v;
        dut.ex1_oper1 = 0;
        dut.eval();
    };
    std::array<uint16_t, 10> class_vals = {0xFC00, 0x8400, 0x8001, 0x8000, 0x0000,
                                            0x0001, 0x0400, 0x7C00, 0x7C01, 0x7E00};
    for (uint16_t v : class_vals) {
        drive_half(v);
        drive_half(0x0000);
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case023/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
