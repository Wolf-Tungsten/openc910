#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fadd_top.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

void tick(Vct_fadd_top& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

uint32_t make_func(bool is_double,
                   bool is_single,
                   bool op_add,
                   bool op_sub,
                   bool op_cmp,
                   bool op_maxnm,
                   bool op_minnm,
                   uint8_t cmp_flags) {
    uint32_t f = 0;
    if (is_double) f |= 1u << 16;
    if (is_single) f |= 1u << 15;
    if (op_add) f |= 1u << 12;
    if (op_sub) f |= 1u << 11;
    if (op_cmp) f |= 1u << 10;
    if (op_maxnm) f |= 1u << 9;
    if (op_minnm) f |= 1u << 8;
    f |= (cmp_flags & 0x1Fu);
    return f;
}

struct Scenario {
    const char* name;
    uint32_t func;
    uint64_t src0;
    uint64_t src1;
    uint64_t expected;
};

uint64_t pack_single(uint32_t low) { return 0xFFFFFFFF00000000ULL | low; }
uint64_t pack_half(uint16_t low) { return 0xFFFFFFFFFFFF0000ULL | low; }
uint64_t pack_single_clean(uint32_t low) { return static_cast<uint64_t>(low); }
uint64_t pack_half_clean(uint16_t low) { return static_cast<uint64_t>(low); }
uint64_t make_double_bits(int exp, uint64_t frac, bool sign = false) {
    return (sign ? (1ULL << 63) : 0) | ((static_cast<uint64_t>(exp) & 0x7FFULL) << 52) | (frac & ((1ULL << 52) - 1));
}
uint64_t make_single_bits(int exp, uint32_t frac, bool sign = false) {
    const uint32_t bits = (sign ? 0x80000000u : 0) | ((static_cast<uint32_t>(exp) & 0xFFu) << 23) | (frac & 0x7FFFFFu);
    return pack_single(bits);
}
uint64_t make_half_bits(int exp, uint16_t frac, bool sign = false) {
    const uint16_t bits = static_cast<uint16_t>((sign ? 0x8000u : 0) | ((exp & 0x1Fu) << 10) | (frac & 0x3FFu));
    return pack_half(bits);
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fadd_top dut;

    auto expect = [&](bool cond, const std::string& msg) {
        if (!cond) {
            std::cerr << msg << "\n";
            return false;
        }
        return true;
    };

    dut.cp0_vfpu_icg_en = 1;
    dut.cp0_yy_clk_en = 1;
    dut.pad_yy_icg_scan_en = 0;
    dut.cpurst_b = 0;
    dut.dp_vfalu_ex1_pipex_sel = 0;
    tick(dut);
    dut.cpurst_b = 1;
    dut.pad_yy_icg_scan_en = 1;
    tick(dut);

    dut.dp_vfalu_ex1_pipex_imm0 = 0;
    dut.vfpu_yy_xx_rm = 0;
    dut.vfpu_yy_xx_dqnan = 0;

    auto run = [&](const Scenario& sc, bool check_expected = true) {
        dut.dp_vfalu_ex1_pipex_func = sc.func;
        dut.dp_vfalu_ex1_pipex_srcf0 = sc.src0;
        dut.dp_vfalu_ex1_pipex_srcf1 = sc.src1;

        dut.dp_vfalu_ex1_pipex_sel = 0b010;  // assert ex1_pipedown
        tick(dut);                           // EX1 -> EX2
        dut.dp_vfalu_ex1_pipex_sel = 0;
        tick(dut);  // EX2 -> EX3

        if (!expect(dut.fadd_forward_r_vld == 1, std::string(sc.name) + " forward valid missing")) return 1;
        if (!expect(dut.fadd_ereg_ex3_forward_r_vld == 1, std::string(sc.name) + " ereg valid missing")) return 1;
        if (check_expected && dut.fadd_forward_result != sc.expected) {
            std::cerr << sc.name << " result mismatch got 0x" << std::hex << dut.fadd_forward_result << " expected 0x"
                      << sc.expected << std::dec << "\n";
            return 1;
        }

        tick(dut);  // let valids drop
        return 0;
    };

    std::vector<Scenario> scenarios = {
        {"double_add", make_func(true, false, true, false, false, false, false, 0), 0x3FF0000000000000ULL, 0x4000000000000000ULL, 0x4008000000000000ULL},                          // 1.0 + 2.0 = 3.0
        {"single_add", make_func(false, true, true, false, false, false, false, 0), pack_single(0x3FC00000), pack_single(0xBF000000), pack_single(0x3F800000)},                      // 1.5 + (-0.5) = 1.0
        {"half_add", make_func(false, false, true, false, false, false, false, 0), pack_half(0x3C00), pack_half(0x3800), pack_half(0x3E00)},                                           // 1.0 + 0.5 = 1.5
        {"double_sub", make_func(true, false, false, true, false, false, false, 0), 0x4008000000000000ULL, 0x3FF0000000000000ULL, 0x4000000000000000ULL},                             // 3.0 - 1.0 = 2.0
        {"single_sub", make_func(false, true, false, true, false, false, false, 0), pack_single(0x40200000), pack_single(0x3F800000), pack_single(0x3FC00000)},                       // 2.5 - 1.0 = 1.5
        {"half_sub", make_func(false, false, false, true, false, false, false, 0), pack_half(0x3C00), pack_half(0x3800), pack_half(0x3800)},                                          // 1.0 - 0.5 = 0.5
    };

    for (const auto& sc : scenarios) {
        if (run(sc)) return 1;
    }

    if (run({"double_maxnm", make_func(true, false, false, false, false, true, false, 0), 0xBFF0000000000000ULL, 0x4008000000000000ULL, 0x4008000000000000ULL})) return 1;
    if (run({"single_minnm", make_func(false, true, false, false, false, false, true, 0), pack_single(0x7F800000), pack_single(0x3F000000), pack_single(0x3F000000)})) return 1;
    if (run({"half_maxnm", make_func(false, false, false, false, false, true, false, 0), pack_half(0xFC00), pack_half(0x3C00), pack_half(0x3C00)})) return 1;

    auto run_cmp = [&](const char* name, uint32_t func, uint64_t s0, uint64_t s1, uint64_t expected_cmp) {
        dut.dp_vfalu_ex1_pipex_func = func;
        dut.dp_vfalu_ex1_pipex_srcf0 = s0;
        dut.dp_vfalu_ex1_pipex_srcf1 = s1;
        dut.dp_vfalu_ex1_pipex_sel = 0b010;
        tick(dut);
        dut.dp_vfalu_ex1_pipex_sel = 0;
        tick(dut);
        if (!expect(dut.fadd_forward_r_vld == 1, std::string(name) + " cmp forward valid missing")) return false;
        if (!expect(dut.fadd_mfvr_cmp_result == expected_cmp, std::string(name) + " cmp result mismatch")) return false;
        tick(dut);
        return true;
    };
    if (!run_cmp("cmp_feq_single", make_func(false, true, false, false, true, false, false, 0b00001), pack_single(0x3F800000), pack_single(0x3F800000), 1)) return 1;
    if (!run_cmp("cmp_flt_double", make_func(true, false, false, false, true, false, false, 0b00010), 0x3FF0000000000000ULL, 0x4008000000000000ULL, 1)) return 1;
    if (!run_cmp("cmp_fle_half", make_func(false, false, false, false, true, false, false, 0b00100), pack_half(0x3800), pack_half(0x3C00), 1)) return 1;
    if (!run_cmp("cmp_unord_double", make_func(true, false, false, false, true, false, false, 0b00001), 0x7FF8000000000001ULL, 0x3FF0000000000000ULL, 0)) return 1;
    if (!run_cmp("cmp_unord_single", make_func(false, true, false, false, true, false, false, 0b00010), pack_single(0x7FC00001), pack_single(0x3F800000), 0)) return 1;
    if (!run_cmp("cmp_unord_half", make_func(false, false, false, false, true, false, false, 0b00100), pack_half(0x7E00), pack_half(0x3C00), 0)) return 1;

    dut.dp_vfalu_ex1_pipex_imm0 = 0b001;  // RTZ
    if (run({"double_rtz_add", make_func(true, false, true, false, false, false, false, 0), 0x3FF0000000000000ULL, 0x3FF0000000000000ULL, 0x4000000000000000ULL})) return 1;
    dut.dp_vfalu_ex1_pipex_imm0 = 0b010;  // RDN
    if (run({"single_rdn_add", make_func(false, true, true, false, false, false, false, 0), pack_single(0x3E99999A), pack_single(0x3E4CCCCD), pack_single(0x3F000000)})) return 1;
    dut.dp_vfalu_ex1_pipex_imm0 = 0b111;  // dynamic
    dut.vfpu_yy_xx_rm = 0b011;            // RUP
    if (run({"single_rup_add", make_func(false, true, true, false, false, false, false, 0), pack_single(0x3E99999A), pack_single(0x3E99999A), pack_single(0x3F19999A)})) return 1;
    dut.vfpu_yy_xx_rm = 0b100;            // RMM
    if (run({"double_rmm_add", make_func(true, false, true, false, false, false, false, 0), 0x3FF0000000000000ULL, 0x3FF0000000000000ULL, 0x4000000000000000ULL})) return 1;
    dut.dp_vfalu_ex1_pipex_imm0 = 0;
    dut.vfpu_yy_xx_rm = 0;

    dut.vfpu_yy_xx_dqnan = 1;
    if (run({"single_nan_add", make_func(false, true, true, false, false, false, false, 0), pack_single(0x7FC00001), pack_single(0x3F800000), pack_single(0x7FC00001)})) return 1;
    dut.vfpu_yy_xx_dqnan = 0;

    // Close-path sweeps to exercise onehot shifters.
    auto close_sweep = [&](bool is_single, bool is_half) {
        for (uint16_t frac = 0x0001; frac < 0x0008; ++frac) {
            uint64_t a = is_single ? pack_single(0x3F800000 + (frac << 7)) : pack_half(static_cast<uint16_t>(0x3C00 + frac));
            uint64_t b = is_single ? pack_single(0x3F800000 - (frac << 6)) : pack_half(static_cast<uint16_t>(0x3C00 - frac));
            uint32_t func = make_func(false, is_single, false, true, false, false, false, 0);
            dut.dp_vfalu_ex1_pipex_func = func;
            dut.dp_vfalu_ex1_pipex_srcf0 = a;
            dut.dp_vfalu_ex1_pipex_srcf1 = b;
            dut.dp_vfalu_ex1_pipex_sel = 0b010;
            tick(dut);
            dut.dp_vfalu_ex1_pipex_sel = 0;
            tick(dut);
            tick(dut);
        }
    };
    close_sweep(true, false);
    close_sweep(false, true);
    // Double close sweep for near-equal operands.
    {
        for (uint16_t frac = 0x0001; frac < 0x0010; ++frac) {
            uint64_t a = 0x3FF0000000000000ULL + (static_cast<uint64_t>(frac) << 40);
            uint64_t b = 0x3FF0000000000000ULL - (static_cast<uint64_t>(frac) << 39);
            uint32_t func = make_func(true, false, false, true, false, false, false, 0);
            dut.dp_vfalu_ex1_pipex_func = func;
            dut.dp_vfalu_ex1_pipex_srcf0 = a;
            dut.dp_vfalu_ex1_pipex_srcf1 = b;
            dut.dp_vfalu_ex1_pipex_sel = 0b010;
            tick(dut);
            dut.dp_vfalu_ex1_pipex_sel = 0;
            tick(dut);
            tick(dut);
        }
    }

    // Drive close-path leading-one positions explicitly.
    auto close_onehot = [&](bool is_double, bool is_single, int max_k) {
        const uint64_t base_exp = is_double ? 0x3FF0000000000000ULL : (is_single ? pack_single(0x3F800000) : pack_half(0x3C00));
        for (int k = 0; k <= max_k; ++k) {
            uint64_t frac_mask = 0;
            if (is_double) {
                frac_mask = 1ULL << k;
            } else if (is_single) {
                frac_mask = static_cast<uint64_t>(1u << k);
            } else {
                frac_mask = static_cast<uint64_t>(1u << k);
            }
            uint64_t a = base_exp | frac_mask;
            uint64_t b = base_exp;
            uint32_t func = make_func(is_double, is_single, false, true, false, false, false, 0);
            dut.dp_vfalu_ex1_pipex_func = func;
            dut.dp_vfalu_ex1_pipex_srcf0 = a;
            dut.dp_vfalu_ex1_pipex_srcf1 = b;
            dut.dp_vfalu_ex1_pipex_sel = 0b010;
            tick(dut);
            dut.dp_vfalu_ex1_pipex_sel = 0;
            tick(dut);
            tick(dut);
        }
    };
    close_onehot(true, false, 51);
    close_onehot(false, true, 22);
    close_onehot(false, false, 9);

    // Cancellation adds with opposite signs to stress close_s1 predictor across bit positions.
    auto cancel_op = [&](bool is_double, bool is_single, int max_k) {
        const uint64_t base = is_double ? 0x3FF0000000000000ULL : (is_single ? pack_single(0x3F800000) : pack_half(0x3C00));
        for (int k = 0; k <= max_k; ++k) {
            uint64_t mask = is_double ? (1ULL << k) : (is_single ? static_cast<uint64_t>(1u << k) : static_cast<uint64_t>(1u << k));
            uint64_t a = base | mask;              // +mag
            uint64_t b = base | mask;              // -mag
            if (is_double) b |= 1ULL << 63;
            else if (is_single) b ^= 0x80000000ULL;
            else b ^= 0x8000ULL;
            uint32_t func = make_func(is_double, is_single, true, false, false, false, false, 0);
            dut.dp_vfalu_ex1_pipex_func = func;
            dut.dp_vfalu_ex1_pipex_srcf0 = a;
            dut.dp_vfalu_ex1_pipex_srcf1 = b;
            dut.dp_vfalu_ex1_pipex_sel = 0b010;
            tick(dut);
            dut.dp_vfalu_ex1_pipex_sel = 0;
            tick(dut);
            tick(dut);
        }
    };
    cancel_op(true, false, 51);
    cancel_op(false, true, 22);
    cancel_op(false, false, 9);

    // Far-path alignment sweeps to hit wide onehot positions.
    auto far_align_sweep = [&](bool is_double, bool is_single, int max_shift) {
        for (int shift = 0; shift <= max_shift; ++shift) {
            uint64_t big = 0;
            uint64_t small = 0;
            if (is_double) {
                big = make_double_bits(0x3FF, 0);
                const int exp_small = 0x3FF - shift;
                const uint64_t frac = 1ULL << (shift % 52);
                small = make_double_bits(exp_small > 0 ? exp_small : 0, frac | 1ULL);
            } else if (is_single) {
                big = pack_single(0x3F800000);
                const int exp_small = 0x7F - shift;
                const uint32_t frac = 1u << (shift % 23);
                small = make_single_bits(exp_small > 0 ? exp_small : 0, frac | 1u);
            } else {
                big = pack_half(0x3C00);
                const int exp_small = 0xF - shift;
                const uint16_t frac = static_cast<uint16_t>(1u << (shift % 10));
                small = make_half_bits(exp_small > 0 ? exp_small : 0, static_cast<uint16_t>(frac | 1u));
            }

            const uint32_t func_add = make_func(is_double, is_single, true, false, false, false, false, 0);
            const uint32_t func_sub = make_func(is_double, is_single, false, true, false, false, false, 0);
            for (uint32_t func : {func_add, func_sub}) {
                dut.dp_vfalu_ex1_pipex_func = func;
                dut.dp_vfalu_ex1_pipex_srcf0 = big;
                dut.dp_vfalu_ex1_pipex_srcf1 = small;
                dut.dp_vfalu_ex1_pipex_sel = 0b010;
                tick(dut);
                dut.dp_vfalu_ex1_pipex_sel = 0;
                tick(dut);
                tick(dut);
            }
        }
    };
    far_align_sweep(true, false, 60);
    far_align_sweep(false, true, 30);
    far_align_sweep(false, false, 20);

    // Additional special cases without strict result checking to exercise NaN/close/far paths.
    std::vector<Scenario> extra_cases = {
        {"double_close_sub", make_func(true, false, false, true, false, false, false, 0), 0x3FF0000000000001ULL, 0x3FF0000000000000ULL, 0},
        {"single_qnan_add", make_func(false, true, true, false, false, false, false, 0), pack_single(0x7FC00001), pack_single(0x3F800000), 0},
        {"half_snan_add", make_func(false, false, true, false, false, false, false, 0), pack_half(0x7DFF), pack_half(0x3C00), 0},
        {"double_inf_minnm", make_func(true, false, false, false, false, false, true, 0), 0x7FF0000000000000ULL, 0x7FF8000000000000ULL, 0},
    };
    for (const auto& sc : extra_cases) {
        if (run(sc, false)) return 1;
    }

    std::vector<Scenario> maxmin_cases = {
        {"double_maxnm_nan", make_func(true, false, false, false, false, true, false, 0), 0x7FF8000000000001ULL, 0x3FF0000000000000ULL, 0},
        {"double_minnm_nan", make_func(true, false, false, false, false, false, true, 0), 0x7FF0000000000001ULL, 0x4000000000000000ULL, 0},
        {"double_maxnm_neg", make_func(true, false, false, false, false, true, false, 0), 0xBFF0000000000000ULL, 0x3FF0000000000000ULL, 0},
        {"double_minnm_neg", make_func(true, false, false, false, false, false, true, 0), 0xBFF0000000000000ULL, 0x3FF0000000000000ULL, 0},
        {"single_maxnm_zero", make_func(false, true, false, false, false, true, false, 0), pack_single(0x00000000), pack_single(0xBF800000), 0},
        {"single_minnm_inf", make_func(false, true, false, false, false, false, true, 0), pack_single(0x7F800000), pack_single(0xFF800000), 0},
    };
    for (const auto& sc : maxmin_cases) {
        if (run(sc, false)) return 1;
    }

    auto drive_op = [&](const char* name, uint32_t func, uint64_t a, uint64_t b) {
        dut.dp_vfalu_ex1_pipex_func = func;
        dut.dp_vfalu_ex1_pipex_srcf0 = a;
        dut.dp_vfalu_ex1_pipex_srcf1 = b;
        dut.dp_vfalu_ex1_pipex_sel = 0b010;
        tick(dut);
        dut.dp_vfalu_ex1_pipex_sel = 0;
        tick(dut);
        if (dut.fadd_forward_r_vld != 1) {
            std::cerr << name << " forward missing\n";
            return false;
        }
        tick(dut);
        return true;
    };

    // Broad classification matrix to toggle inf/NaN/zero/denorm paths.
    const uint64_t double_vals[] = {
        0x0000000000000000ULL,  // +0
        0x8000000000000000ULL,  // -0
        0x7FF0000000000000ULL,  // +inf
        0xFFF0000000000000ULL,  // -inf
        0x7FF8000000000000ULL,  // qNaN
        0x7FF0000000000001ULL,  // sNaN
        0x3FF0000000000000ULL,  // 1.0
        0xBFF0000000000000ULL,  // -1.0
        0x0010000000000000ULL,  // min normal
        0x0000000000000001ULL,  // min subnormal
        0x7FEFFFFFFFFFFFFFULL,  // max finite
        0xFFEFFFFFFFFFFFFFULL   // max finite negative
    };
    for (uint64_t a : double_vals) {
        for (uint64_t b : double_vals) {
            drive_op("combo_add", make_func(true, false, true, false, false, false, false, 0), a, b);
            drive_op("combo_sub", make_func(true, false, false, true, false, false, false, 0), a, b);
            drive_op("combo_maxnm", make_func(true, false, false, false, false, true, false, 0), a, b);
            drive_op("combo_minnm", make_func(true, false, false, false, false, false, true, 0), a, b);
        }
    }

    // Denorm close interactions to exercise type1/2 predictor paths.
    for (int k = 0; k < 20; ++k) {
        uint64_t frac_a = 1ULL << (k % 52);
        uint64_t frac_b = (k + 1) < 52 ? 1ULL << ((k + 1) % 52) : 1;
        uint64_t a = make_double_bits(0, frac_a);
        uint64_t b = make_double_bits(0, frac_b);
        drive_op("denorm_close_add", make_func(true, false, true, false, false, false, false, 0), a, b);
        drive_op("denorm_close_sub", make_func(true, false, false, true, false, false, false, 0), a, b);
    }
    for (int k = 0; k < 20; ++k) {
        uint16_t frac_a = static_cast<uint16_t>(1u << (k % 10));
        uint16_t frac_b = static_cast<uint16_t>(1u << ((k + 1) % 10));
        uint64_t a = pack_half(static_cast<uint16_t>(frac_a));
        uint64_t b = pack_half(static_cast<uint16_t>(frac_b));
        drive_op("half_denorm_close_add", make_func(false, false, true, false, false, false, false, 0), a, b);
        drive_op("half_denorm_close_sub", make_func(false, false, false, true, false, false, false, 0), a, b);
    }

    // Dense random close-path exploration for double and half.
    uint64_t lcg_close = 123;
    auto next_close = [&]() {
        lcg_close = lcg_close * 6364136223846793005ULL + 1;
        return lcg_close;
    };
    for (int i = 0; i < 5000; ++i) {
        uint64_t frac_a = next_close() & ((1ULL << 52) - 1);
        uint64_t frac_b = next_close() & ((1ULL << 52) - 1);
        uint64_t a = make_double_bits(0x3FF, frac_a);
        uint64_t b = make_double_bits(0x3FF, frac_b);
        uint32_t func = make_func(true, false, false, true, false, false, false, 0);
        drive_op("close_rand_sub", func, a, b);
    }
    for (int i = 0; i < 2048; ++i) {
        uint16_t frac_a = static_cast<uint16_t>(next_close() & 0x3FF);
        uint16_t frac_b = static_cast<uint16_t>(next_close() & 0x3FF);
        uint64_t a = pack_half(static_cast<uint16_t>(0x3C00 | frac_a));
        uint64_t b = pack_half(static_cast<uint16_t>(0x3C00 | frac_b));
        drive_op("half_close_rand", make_func(false, false, false, true, false, false, false, 0), a, b);
    }

    // Target far-path rounding corner cases with varying exponent gaps, fractions, and round modes.
    auto far_sub_round_sweep = [&]() {
        const int exp_big_list[] = {0x400, 0x3FF, 0x200};
        const uint64_t frac_list[] = {0ULL, 1ULL << 51, 1ULL << 50, 0x000FFFFFFFFFFFFULL, 0x0000000000000001ULL};
        const uint8_t rm_list[] = {0b000, 0b001, 0b010, 0b011, 0b100};  // RNE, RTZ, RDN, RUP, RMM
        for (int exp_big : exp_big_list) {
            for (int gap = 2; gap <= 10; ++gap) {
                int exp_small = exp_big - gap;
                if (exp_small <= 0) continue;
                for (uint64_t frac : frac_list) {
                    uint64_t a = make_double_bits(exp_big, 0);
                    uint64_t b = make_double_bits(exp_small, frac);
                    for (uint8_t rm : rm_list) {
                        dut.dp_vfalu_ex1_pipex_imm0 = rm;
                        dut.vfpu_yy_xx_rm = rm;
                        drive_op("far_sub_round", make_func(true, false, false, true, false, false, false, 0), a, b);
                    }
                }
            }
        }
        // Mix in sign variations.
        for (uint64_t frac : {0ULL, 1ULL << 52, 0x0008000000000000ULL}) {
            uint64_t a = make_double_bits(0x3FF, frac, true);   // negative larger
            uint64_t b = make_double_bits(0x3F0, frac >> 1, false);
            for (uint8_t rm : rm_list) {
                dut.dp_vfalu_ex1_pipex_imm0 = rm;
                dut.vfpu_yy_xx_rm = rm;
                drive_op("far_sub_round_sign", make_func(true, false, false, true, false, false, false, 0), a, b);
            }
        }
        dut.dp_vfalu_ex1_pipex_imm0 = 0;
        dut.vfpu_yy_xx_rm = 0;
    };
    far_sub_round_sweep();

    // Extra qNaN/Inf/zero permutations to hit special result muxes and sign choices.
    dut.vfpu_yy_xx_dqnan = 1;
    std::vector<Scenario> qnan_cases = {
        {"double_qnan_src0", make_func(true, false, true, false, false, false, false, 0), 0x7FF8000000000001ULL, 0x3FF0000000000000ULL, 0},
        {"double_qnan_src1", make_func(true, false, true, false, false, false, false, 0), 0x3FF0000000000000ULL, 0x7FF8000000000001ULL, 0},
        {"double_snan_src1", make_func(true, false, true, false, false, false, false, 0), 0x3FF0000000000000ULL, 0x7FF0000000000001ULL, 0},
        {"double_neg_qnan", make_func(true, false, true, false, false, false, false, 0), 0x3FF8000000000000ULL, 0xFFF8000000000001ULL, 0},
        {"double_overflow", make_func(true, false, true, false, false, false, false, 0), 0x7FEFFFFFFFFFFFFFULL, 0x7FEFFFFFFFFFFFFFULL, 0},
        {"double_cancel_to_zero", make_func(true, false, true, false, false, false, false, 0), 0x3FF0000000000000ULL, 0xBFF0000000000000ULL, 0},
        {"half_inf_add", make_func(false, false, true, false, false, false, false, 0), pack_half(0x7C00), pack_half(0x0400), 0},
        {"half_lfn_add", make_func(false, false, true, false, false, false, false, 0), pack_half(0x7BFF), pack_half(0x3C00), 0},
        {"half_rhs_inf", make_func(false, false, true, false, false, false, false, 0), pack_half(0x3C00), pack_half(0x7C00), 0},
        {"half_rhs_lfn", make_func(false, false, true, false, false, false, false, 0), pack_half(0x3C00), pack_half(0x7BFF), 0},
        {"half_zero_mix", make_func(false, false, true, false, false, false, false, 0), pack_half(0x0000), pack_half(0x8000), 0},
    };
    for (const auto& sc : qnan_cases) {
        if (run(sc, false)) return 1;
    }
    dut.vfpu_yy_xx_dqnan = 0;

    // Half NaN/Inf with dqnan toggled to hit qnan sign/data paths.
    dut.vfpu_yy_xx_dqnan = 1;
    std::vector<Scenario> half_qnan_paths = {
        {"half_qnan_dqnan", make_func(false, false, true, false, false, false, false, 0), pack_half(0x7E01), pack_half(0x3C00), 0},
        {"half_snan_dqnan", make_func(false, false, true, false, false, false, false, 0), pack_half(0x7D01), pack_half(0xBC00), 0},
        {"half_qnan_neg", make_func(false, false, true, false, false, false, false, 0), pack_half(0xFE01), pack_half(0x3C00), 0},
    };
    for (const auto& sc : half_qnan_paths) {
        if (run(sc, false)) return 1;
    }
    dut.vfpu_yy_xx_dqnan = 0;

    // Drive denorm-close normalization shifts for double path.
    auto denorm_close = [&](int max_k) {
        const int exp = 1;
        for (int k = 1; k <= max_k; ++k) {
            uint64_t a = make_double_bits(exp, 1ULL << (k % 52));
            uint64_t b = make_double_bits(exp, 0);
            dut.dp_vfalu_ex1_pipex_func = make_func(true, false, false, true, false, false, false, 0);
            dut.dp_vfalu_ex1_pipex_srcf0 = a;
            dut.dp_vfalu_ex1_pipex_srcf1 = b;
            dut.dp_vfalu_ex1_pipex_sel = 0b010;
            tick(dut);
            dut.dp_vfalu_ex1_pipex_sel = 0;
            tick(dut);
            tick(dut);
        }
    };
    denorm_close(51);

    // Sweep very small exponents to exercise denorm shift table and onehot LSBs.
    auto small_exp_walk = [&]() {
        for (int exp = 0; exp <= 60; ++exp) {
            uint64_t a = make_double_bits(exp, 1ULL << (exp % 52));
            uint64_t b = make_double_bits(exp, 0);
            dut.dp_vfalu_ex1_pipex_func = make_func(true, false, true, false, false, false, false, 0);
            dut.dp_vfalu_ex1_pipex_srcf0 = a;
            dut.dp_vfalu_ex1_pipex_srcf1 = b;
            dut.dp_vfalu_ex1_pipex_sel = 0b010;
            tick(dut);
            dut.dp_vfalu_ex1_pipex_sel = 0;
            tick(dut);
            tick(dut);
        }
    };
    small_exp_walk();

    // Half-specific edge cases to toggle inf/lfn/zero paths.
    std::vector<Scenario> half_edges = {
        {"half_inf_inf_add", make_func(false, false, true, false, false, false, false, 0), pack_half(0x7C00), pack_half(0x7C00), 0},
        {"half_lfn_lfn_add", make_func(false, false, true, false, false, false, false, 0), pack_half(0x7BFF), pack_half(0x7BFF), 0},
        {"half_inf_sub", make_func(false, false, false, true, false, false, false, 0), pack_half(0x7C00), pack_half(0x3C00), 0},
        {"half_zero_sub", make_func(false, false, false, true, false, false, false, 0), pack_half(0x0000), pack_half(0x0001), 0},
        {"half_dn_sub", make_func(false, false, false, true, false, false, false, 0), pack_half(0x0002), pack_half(0x0001), 0},
        {"half_dn_add", make_func(false, false, true, false, false, false, false, 0), pack_half(0x0002), pack_half(0x0003), 0},
    };
    for (const auto& sc : half_edges) {
        if (run(sc, false)) return 1;
    }

    // Half NaN sign propagation with dqnan disabled.
    std::vector<Scenario> half_nan_sign = {
        {"half_qnan_neg", make_func(false, false, true, false, false, false, false, 0), pack_half(0xFE01), pack_half(0x3C00), 0},
        {"half_qnan_rhs", make_func(false, false, true, false, false, false, false, 0), pack_half(0x3C00), pack_half(0x7E01), 0},
    };
    for (const auto& sc : half_nan_sign) {
        if (run(sc, false)) return 1;
    }

    // Single canonical layouts (upper bits zero) to trigger cNaN/Inf/zero detection.
    std::vector<Scenario> single_flag_cases = {
        {"single_clean_zero", make_func(false, true, true, false, false, false, false, 0), pack_single_clean(0x00000000), pack_single_clean(0x3F800000), pack_single_clean(0x3F800000)},
        {"single_clean_neg_zero", make_func(false, true, true, false, false, false, false, 0), pack_single_clean(0x80000000), pack_single_clean(0x3F800000), pack_single_clean(0xBF800000)},
        {"single_clean_inf", make_func(false, true, true, false, false, false, false, 0), pack_single_clean(0x7F800000), pack_single_clean(0x40000000), 0},
        {"single_clean_snan", make_func(false, true, true, false, false, false, false, 0), pack_single_clean(0x7F800001), pack_single_clean(0x3F800000), 0},
        {"single_clean_qnan", make_func(false, true, true, false, false, false, false, 0), pack_single_clean(0x7FC00000), pack_single_clean(0x3F800000), 0},
    };
    for (const auto& sc : single_flag_cases) {
        if (run(sc, false)) return 1;
    }

    // Extra double NaN mixes with dqnan disabled to flip qnan sign muxes.
    dut.vfpu_yy_xx_dqnan = 0;
    std::vector<Scenario> double_nan_sign = {
        {"double_qnan_both", make_func(true, false, true, false, false, false, false, 0), 0x7FF8000000000001ULL, 0x7FF8000000000002ULL, 0},
        {"double_snan_both", make_func(true, false, true, false, false, false, false, 0), 0x7FF0000000000001ULL, 0x7FF0000000000002ULL, 0},
    };
    for (const auto& sc : double_nan_sign) {
        if (run(sc, false)) return 1;
    }

    // Toggle gates and run a burst of random ops for coverage.
    dut.cp0_yy_clk_en = 0;
    tick(dut);
    dut.cp0_yy_clk_en = 1;
    dut.cp0_vfpu_icg_en = 0;
    tick(dut);
    dut.cp0_vfpu_icg_en = 1;

    uint64_t lcg = 9;
    auto next = [&]() {
        lcg = lcg * 6364136223846793005ULL + 1;
        return lcg;
    };
    for (int i = 0; i < 100000; ++i) {
        const bool is_double = (i % 3) == 0;
        const bool is_single = (i % 3) == 1;
        const bool is_half = !is_double && !is_single;
        uint32_t func = make_func(is_double, is_single, (i & 1), (i & 2), (i & 4), (i & 8), (i & 16), static_cast<uint8_t>(i));
        dut.dp_vfalu_ex1_pipex_func = func;
        if (is_double) {
            dut.dp_vfalu_ex1_pipex_srcf0 = next();
            dut.dp_vfalu_ex1_pipex_srcf1 = next();
        } else if (is_single) {
            dut.dp_vfalu_ex1_pipex_srcf0 = pack_single(static_cast<uint32_t>(next()));
            dut.dp_vfalu_ex1_pipex_srcf1 = pack_single(static_cast<uint32_t>(next()));
        } else {
            dut.dp_vfalu_ex1_pipex_srcf0 = pack_half(static_cast<uint16_t>(next()));
            dut.dp_vfalu_ex1_pipex_srcf1 = pack_half(static_cast<uint16_t>(next()));
        }
        dut.dp_vfalu_ex1_pipex_imm0 = next() & 0x7;
        dut.vfpu_yy_xx_rm = next() & 0x7;
        dut.dp_vfalu_ex1_pipex_sel = 0b010;
        tick(dut);
        dut.dp_vfalu_ex1_pipex_sel = 0;
        tick(dut);
        tick(dut);
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case028/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
