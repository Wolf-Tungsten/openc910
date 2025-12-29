#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fcnvt_top.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

uint64_t pack_single(uint32_t low) { return 0xFFFFFFFF00000000ULL | low; }
uint64_t pack_half(uint16_t low) { return 0xFFFFFFFFFFFF0000ULL | low; }
uint64_t make_double(int sign, uint16_t exp, uint64_t frac) { return (static_cast<uint64_t>(sign & 1) << 63) | (static_cast<uint64_t>(exp & 0x7FF) << 52) | (frac & ((1ULL << 52) - 1)); }

void tick(Vct_fcnvt_top& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fcnvt_top dut;

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

    auto func_bits = [&](bool src_l64, bool src_l32, bool widden, bool narrow, bool src_si, bool src_float, bool dest_float,
                         bool dest_si, bool dest_ui, bool dest_h, bool dest_s) {
        uint32_t f = 0;
        if (src_l64) f |= 1u << 16;
        if (src_l32) f |= 1u << 15;
        if (widden) f |= 1u << 14;
        if (narrow) f |= 1u << 13;
        if (src_si) f |= 1u << 0;
        if (src_float) f |= 1u << 1;
        if (dest_float) f |= 1u << 2;
        if (dest_si) f |= 1u << 3;
        if (dest_ui) f |= 1u << 4;
        if (dest_h) f |= 1u << 10;
        if (dest_s) f |= 1u << 11;
        return f;
    };

    auto run = [&](const char* name, uint32_t func, uint64_t src, uint8_t imm, uint8_t rm, bool dqnan, bool expect_nonzero) {
        dut.dp_vfalu_ex1_pipex_func = func;
        dut.dp_vfalu_ex1_pipex_srcf0 = src;
        dut.dp_vfalu_ex1_pipex_imm0 = imm & 0x7;
        dut.vfpu_yy_xx_rm = rm & 0x7;
        dut.vfpu_yy_xx_dqnan = dqnan ? 1 : 0;
        dut.dp_vfalu_ex1_pipex_sel = 0b100;
        tick(dut);  // EX1 -> EX2
        dut.dp_vfalu_ex1_pipex_sel = 0;
        tick(dut);  // EX2 -> EX3
        if (!expect(dut.fcnvt_forward_r_vld == 1, std::string(name) + " forward valid missing")) return false;
        if (expect_nonzero && !expect(dut.fcnvt_forward_result != 0, std::string(name) + " result stuck zero")) return false;
        tick(dut);
        return true;
    };

    if (!run("double_passthrough", func_bits(true, false, false, false, false, true, true, false, false, false, false),
             0x4008000000000000ULL, 0, 0, false, true))
        return 1;
    if (!run("double_to_int", func_bits(true, false, false, false, false, true, false, true, false, false, false), 0x3FF0000000000000ULL,
             0, 1, false, true))
        return 1;
    if (!run("double_to_uint", func_bits(true, false, false, false, false, true, false, false, true, false, false),
             0x3FF8000000000000ULL, 1, 2, false, true))
        return 1;
    if (!run("double_to_half_narrow", func_bits(true, false, false, true, false, true, true, false, false, true, false),
             0x3FF8000000000000ULL, 2, 3, false, true))
        return 1;
    if (!run("double_to_single_narrow", func_bits(true, false, false, true, false, true, true, false, false, false, true),
             0x4000000000000000ULL, 3, 4, true, true))
        return 1;
    if (!run("single_to_double_widen", func_bits(false, true, true, false, false, true, true, false, false, false, false),
             pack_single(0x3F800000), 4, 5, false, true))
        return 1;
    if (!run("single_to_half", func_bits(false, true, false, true, false, true, true, false, false, true, false), pack_single(0x40000000),
             5, 6, false, true))
        return 1;
    if (!run("half_to_single", func_bits(false, false, true, false, false, false, true, false, false, false, true), pack_half(0x3E00), 6, 7,
             true, true))
        return 1;
    if (!run("half_to_int", func_bits(false, false, false, false, false, false, false, true, false, false, false), pack_half(0x3800), 7, 0,
             false, false))
        return 1;
    if (!run("half_to_uint", func_bits(false, false, false, false, false, false, false, false, true, false, false), pack_half(0xBC00), 0, 0,
             false, false))
        return 1;
    if (!run("double_nan_dqnan", func_bits(true, false, false, false, false, true, false, false, false, false, false), 0x7FF8000000000000ULL,
             0, 0, true, false))
        return 1;
    if (!run("double_to_half_denorm", func_bits(true, false, false, true, false, true, true, false, false, true, false),
             0x0008000000000000ULL, 1, 2, false, false))
        return 1;
    if (!run("double_to_half_overflow", func_bits(true, false, false, true, false, true, true, false, false, true, false),
             0x7FEFFFFFFFFFFFFFULL, 2, 3, false, true))
        return 1;
    if (!run("double_to_single_denorm", func_bits(true, false, false, true, false, true, true, false, false, false, true),
             0x0010000000000000ULL, 3, 4, false, false))
        return 1;
    if (!run("double_to_single_overflow", func_bits(true, false, false, true, false, true, true, false, false, false, true),
             0x7FEFFFFFFFFFFFFFULL, 4, 5, false, true))
        return 1;
    if (!run("sover_toggle", func_bits(true, false, true, true, false, true, true, false, false, false, true), 0x3FF0000000000000ULL, 5, 6,
             true, true))
        return 1;

    // Focused sweeps for double->half and double->single shift/round logic.
    for (int sign : {0, 1}) {
        for (int exp : {0, 1, 2, 5, 10, 20, 50, 100, 200, 400, 800, 1000, 1022, 1023}) {
            for (uint64_t frac : {0ULL, 1ULL << 51, 0x000FFFFFFFFFFFFULL, 0x001000000000000ULL, 0x000800000000000ULL}) {
                uint64_t val = make_double(sign, static_cast<uint16_t>(exp), frac);
                uint8_t rm = static_cast<uint8_t>((exp + sign) & 0x7);
                (void)run("dtoh_sweep", func_bits(true, false, false, true, false, true, true, false, false, true, false), val, 0, rm, false,
                          false);
                (void)run("dtos_sweep", func_bits(true, false, false, true, false, true, true, false, false, false, true), val, 1, rm, false,
                          false);
            }
        }
    }

    // Exhaustive exponent walks to hit dtoh/dtos shifter tables for deep underflow paths.
    for (int exp = 850; exp <= 1023; ++exp) {
        uint64_t frac = (exp & 1) ? (1ULL << 51) : 0x000123456789ABULL;
        uint64_t val_pos = make_double(0, static_cast<uint16_t>(exp), frac);
        uint64_t val_neg = make_double(1, static_cast<uint16_t>(exp), frac >> 1);
        (void)run("dtoh_exp_walk_pos", func_bits(true, false, false, true, false, true, true, false, false, true, false), val_pos, 0, 0, false,
                  false);
        (void)run("dtoh_exp_walk_neg", func_bits(true, false, false, true, false, true, true, false, false, true, false), val_neg, 0, 1, false,
                  false);
        (void)run("dtos_exp_walk_pos", func_bits(true, false, false, true, false, true, true, false, false, false, true), val_pos, 1, 2, false,
                  false);
        (void)run("dtos_exp_walk_neg", func_bits(true, false, false, true, false, true, true, false, false, false, true), val_neg, 1, 3, false,
                  false);
    }

    // Single denorm → double widen: walk leading-one positions to hit stod_sh cases.
    for (int bit = 0; bit <= 22; ++bit) {
        uint32_t frac = 1u << bit;
        uint64_t single_denorm = pack_single(frac);  // exponent=0
        (void)run("stod_denorm_walk", func_bits(false, true, true, false, false, true, true, false, false, false, false), single_denorm, 0, 0,
                  false, false);
    }

    // Int -> float leading-one sweep to cover itof shifter cases.
    for (int bit = 0; bit < 64; ++bit) {
        uint64_t pos = 1ULL << bit;
        uint64_t neg = pos | (1ULL << 63);
        (void)run("itod_pos", func_bits(true, false, false, false, true, false, true, false, false, false, false), pos, 0, 0, false, true);
        (void)run("itod_neg", func_bits(true, false, false, false, true, false, true, false, false, false, false), neg, 0, 0, false, true);
        (void)run("itos_pos", func_bits(true, false, false, false, true, false, true, false, false, false, true), pos, 0, 1, false, true);
        (void)run("itos_neg", func_bits(true, false, false, false, true, false, true, false, false, false, true), neg, 0, 1, false, true);
        (void)run("itoh_pos", func_bits(true, false, false, false, true, false, true, false, false, true, false), pos, 0, 2, false, true);
        (void)run("itoh_neg", func_bits(true, false, false, false, true, false, true, false, false, true, false), neg, 0, 2, false, true);
    }

    // Randomized toggling for coverage.
    uint64_t lcg = 11;
    auto next = [&]() {
        lcg = lcg * 6364136223846793005ULL + 1;
        return lcg;
    };
    for (int i = 0; i < 4096; ++i) {
        const bool src_l64 = (i & 1) == 0;
        const bool src_l32 = (i & 1) != 0;
        const bool widden = (i & 2) != 0;
        const bool narrow = (i & 4) != 0;
        const bool src_si = (i & 8) != 0;
        const bool src_float = (i & 16) != 0;
        const bool dest_float = (i & 32) != 0;
        const bool dest_si = (i & 64) != 0;
        const bool dest_ui = (i & 128) != 0;
        const bool dest_h = (i & 256) != 0;
        const bool dest_s = (i & 512) != 0;
        uint32_t func = func_bits(src_l64, src_l32, widden, narrow, src_si, src_float, dest_float, dest_si, dest_ui, dest_h, dest_s);
        dut.dp_vfalu_ex1_pipex_func = func;
        dut.dp_vfalu_ex1_pipex_imm0 = next() & 0x7;
        dut.dp_vfalu_ex1_pipex_sel = 0b100;
        if (src_float) {
            if (dest_h) {
                dut.dp_vfalu_ex1_pipex_srcf0 = pack_half(static_cast<uint16_t>(next()));
            } else {
                dut.dp_vfalu_ex1_pipex_srcf0 = pack_single(static_cast<uint32_t>(next()));
            }
        } else {
            dut.dp_vfalu_ex1_pipex_srcf0 = next();
        }
        tick(dut);
        dut.dp_vfalu_ex1_pipex_sel = 0;
        tick(dut);
        tick(dut);
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case029/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
