#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fspu_top.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

void tick(Vct_fspu_top& dut) {
    dut.forever_cpuclk = 0;
    dut.eval();
    dut.forever_cpuclk = 1;
    dut.eval();
}

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

uint16_t fclass_half_bits(uint16_t v) {
    const bool sign = (v >> 15) & 1;
    const uint16_t exp = (v >> 10) & 0x1F;
    const uint16_t frac = v & 0x3FF;
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

uint64_t apply_fsgnj(uint64_t oper0, uint64_t oper1, int width, int sign_lsb) {
    const uint64_t mask = (width == 64) ? ~0ULL : ((1ULL << width) - 1);
    const uint64_t payload_mask = mask >> 1;
    const uint64_t op0 = oper0 & mask;
    const uint64_t op1 = oper1 & mask;
    const uint64_t sign1 = (op1 >> sign_lsb) & 1ULL;
    const uint64_t payload = op0 & payload_mask;
    return (sign1 << sign_lsb) | payload;
}

uint64_t apply_fsgnjn(uint64_t oper0, uint64_t oper1, int width, int sign_lsb) {
    const uint64_t mask = (width == 64) ? ~0ULL : ((1ULL << width) - 1);
    const uint64_t payload_mask = mask >> 1;
    const uint64_t op0 = oper0 & mask;
    const uint64_t op1 = oper1 & mask;
    const uint64_t sign1 = (~op1 >> sign_lsb) & 1ULL;
    const uint64_t payload = op0 & payload_mask;
    return (sign1 << sign_lsb) | payload;
}

uint64_t apply_fsgnjx(uint64_t oper0, uint64_t oper1, int width, int sign_lsb) {
    const uint64_t mask = (width == 64) ? ~0ULL : ((1ULL << width) - 1);
    const uint64_t payload_mask = mask >> 1;
    const uint64_t op0 = oper0 & mask;
    const uint64_t op1 = oper1 & mask;
    const uint64_t sign = ((op0 >> sign_lsb) ^ (op1 >> sign_lsb)) & 1ULL;
    const uint64_t payload = op0 & payload_mask;
    return (sign << sign_lsb) | payload;
}

uint64_t pack_single_val(uint32_t v) { return (0xFFFFFFFFULL << 32) | v; }
uint64_t pack_half_val(uint16_t v) { return (0xFFFFFFFFFFFFULL << 16) | v; }

uint32_t build_func(bool is_double, bool is_single, bool op_class, bool fmvfx, bool fmvxf, bool fsgnj, bool fsgnjn, bool fsgnjx) {
    uint32_t f = 0;
    if (is_double) f |= 1u << 16;
    if (is_single) f |= 1u << 15;
    if (op_class) f |= 1u << 18;
    if (fmvfx) {
        f |= 1u << 5;
        f |= 1u << 0;
    }
    if (fmvxf) {
        f |= 1u << 5;
        f |= 1u << 2;
    }
    if (fsgnj) {
        f |= 1u << 6;
        f |= 1u << 0;
    }
    if (fsgnjn) {
        f |= 1u << 6;
        f |= 1u << 1;
    }
    if (fsgnjx) {
        f |= 1u << 6;
        f |= 1u << 2;
    }
    return f;
}

struct Scenario {
    const char* name;
    uint32_t func;
    uint64_t src0;
    uint64_t src1;
    uint64_t mtvr;
};

struct Expected {
    uint64_t result;
    uint64_t mfvr;
};

Expected compute_expected(const Scenario& sc) {
    const bool is_double = sc.func & (1u << 16);
    const bool is_single = sc.func & (1u << 15);
    const bool op_class = sc.func & (1u << 18);
    const bool op_fmvfx = (sc.func & (1u << 5)) && (sc.func & 1u);
    const bool op_fmvxf = (sc.func & (1u << 5)) && (sc.func & (1u << 2));
    const bool op_fsgnj = (sc.func & (1u << 6)) && (sc.func & 1u);
    const bool op_fsgnjn = (sc.func & (1u << 6)) && (sc.func & (1u << 1));
    const bool op_fsgnjx = (sc.func & (1u << 6)) && (sc.func & (1u << 2));
    const uint64_t chosen_mtvr = op_fmvfx ? sc.mtvr : sc.src1;

    Expected e{0, 0};
    if (is_double) {
        if (op_fmvfx) {
            e.result = chosen_mtvr;
        } else if (op_fsgnj) {
            e.result = apply_fsgnj(sc.src0, sc.src1, 64, 63);
        } else if (op_fsgnjn) {
            e.result = apply_fsgnjn(sc.src0, sc.src1, 64, 63);
        } else if (op_fsgnjx) {
            e.result = apply_fsgnjx(sc.src0, sc.src1, 64, 63);
        }
        e.mfvr = op_class ? make_fclass_double(sc.src0) : (op_fmvxf ? sc.src0 : 0);
    } else if (is_single) {
        const bool cnan0 = (sc.src0 >> 32) != 0xFFFFFFFFULL;
        const bool cnan1 = (sc.src1 >> 32) != 0xFFFFFFFFULL;
        const uint32_t op0 = cnan0 ? 0x7fc00000u : static_cast<uint32_t>(sc.src0);
        const uint32_t op1 = cnan1 ? 0x7fc00000u : static_cast<uint32_t>(sc.src1);
        if (op_fmvfx) {
            e.result = (0xFFFFFFFFULL << 32) | static_cast<uint32_t>(chosen_mtvr);
        } else if (op_fsgnj) {
            e.result = (0xFFFFFFFFULL << 32) | apply_fsgnj(op0, op1, 32, 31);
        } else if (op_fsgnjn) {
            e.result = (0xFFFFFFFFULL << 32) | apply_fsgnjn(op0, op1, 32, 31);
        } else if (op_fsgnjx) {
            e.result = (0xFFFFFFFFULL << 32) | apply_fsgnjx(op0, op1, 32, 31);
        }
        e.mfvr = op_class ? static_cast<uint64_t>(fclass_single_bits(op0))
                          : (op_fmvxf ? static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(sc.src0 & 0xFFFFFFFFu))) : 0);
    } else {
        const bool cnan0 = (sc.src0 >> 16) != 0xFFFFFFFFFFFFULL;
        const bool cnan1 = (sc.src1 >> 16) != 0xFFFFFFFFFFFFULL;
        const uint16_t op0 = cnan0 ? static_cast<uint16_t>(0x7e00) : static_cast<uint16_t>(sc.src0 & 0xFFFF);
        const uint16_t op1 = cnan1 ? static_cast<uint16_t>(0x7e00) : static_cast<uint16_t>(sc.src1 & 0xFFFF);
        if (op_fmvfx) {
            e.result = (0xFFFFFFFFFFFFULL << 16) | static_cast<uint16_t>(chosen_mtvr & 0xFFFF);
        } else if (op_fsgnj) {
            e.result = (0xFFFFFFFFFFFFULL << 16) | static_cast<uint16_t>(apply_fsgnj(op0, op1, 16, 15));
        } else if (op_fsgnjn) {
            e.result = (0xFFFFFFFFFFFFULL << 16) | static_cast<uint16_t>(apply_fsgnjn(op0, op1, 16, 15));
        } else if (op_fsgnjx) {
            e.result = (0xFFFFFFFFFFFFULL << 16) | static_cast<uint16_t>(apply_fsgnjx(op0, op1, 16, 15));
        }
        e.mfvr = op_class ? static_cast<uint64_t>(fclass_half_bits(op0))
                          : (op_fmvxf ? static_cast<uint64_t>(static_cast<int64_t>(static_cast<int16_t>(sc.src0 & 0xFFFF))) : 0);
    }
    return e;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fspu_top dut;

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
    tick(dut);
    dut.cpurst_b = 1;
    dut.pad_yy_icg_scan_en = 1;
    tick(dut);

    std::vector<Scenario> scenarios = {
        {"double_fsgnj", build_func(true, false, false, false, false, true, false, false), 0x0123456789ABCDEFULL, 0xFEDCBA9876543210ULL, 0},
        {"single_class", build_func(false, true, true, false, false, false, false, false), pack_single_val(0x7FC00000u), 0, 0},
        {"half_fmvfx", build_func(false, false, false, true, false, false, false, false), pack_half_val(0x1234u), 0, 0x1122334455667788ULL},
        {"double_fmvfx", build_func(true, false, false, true, false, false, false, false), 0, 0, 0xDEADBEEFCAFEBABEULL},
        {"single_fmvfx", build_func(false, true, false, true, false, false, false, false), 0, 0, 0x89ABCDEF11223344ULL},
        {"double_fmvxf", build_func(true, false, false, false, true, false, false, false), 0x7FF8000000000001ULL, 0, 0},
        {"single_fmvxf", build_func(false, true, false, false, true, false, false, false), pack_single_val(0xFF800001u), 0, 0},
        {"half_fmvxf", build_func(false, false, false, false, true, false, false, false), pack_half_val(0x7DFFu), 0, 0},
        {"double_class_zero", build_func(true, false, true, false, false, false, false, false), 0x8000000000000000ULL, 0, 0},
        {"double_class_posinf", build_func(true, false, true, false, false, false, false, false), 0x7FF0000000000000ULL, 0, 0},
        {"double_class_dn", build_func(true, false, true, false, false, false, false, false), 0x0000000000000001ULL, 0, 0},
        {"single_class_neginf", build_func(false, true, true, false, false, false, false, false), pack_single_val(0xFF800000u), 0, 0},
        {"half_class_qnan", build_func(false, false, true, false, false, false, false, false), pack_half_val(0x7E01u), 0, 0},
        {"half_class_dn", build_func(false, false, true, false, false, false, false, false), pack_half_val(0x0001u), 0, 0},
        {"single_class_negdn", build_func(false, true, true, false, false, false, false, false), pack_single_val(0x80000001u), 0, 0},
        {"single_class_posnm", build_func(false, true, true, false, false, false, false, false), pack_single_val(0x3F800000u), 0, 0},
        {"double_class_snan", build_func(true, false, true, false, false, false, false, false), 0x7FF0000000000001ULL, 0, 0},
        {"single_fsgnjn", build_func(false, true, false, false, false, false, true, false), pack_single_val(0x3F800000u), pack_single_val(0xBF800000u), 0},
        {"single_fsgnjx", build_func(false, true, false, false, false, false, false, true), pack_single_val(0xBF800000u), pack_single_val(0xBF800000u), 0},
        {"single_fsgnj", build_func(false, true, false, false, false, true, false, false), pack_single_val(0xBF800000u), pack_single_val(0x3F800000u), 0},
        {"half_fsgnj", build_func(false, false, false, false, false, true, false, false), pack_half_val(0x3C00u), pack_half_val(0xBC00u), 0},
        {"half_fsgnjn", build_func(false, false, false, false, false, false, true, false), pack_half_val(0x3C00u), pack_half_val(0xBC00u), 0},
        {"half_fsgnjx", build_func(false, false, false, false, false, false, false, true), pack_half_val(0x3C00u), pack_half_val(0xBC00u), 0},
    };

    for (const auto& sc : scenarios) {
        Expected exp = compute_expected(sc);
        dut.dp_vfalu_ex1_pipex_func = sc.func;
        dut.dp_vfalu_ex1_pipex_srcf0 = sc.src0;
        dut.dp_vfalu_ex1_pipex_srcf1 = sc.src1;
        dut.dp_vfalu_ex1_pipex_mtvr_src0 = sc.mtvr;

        dut.dp_vfalu_ex1_pipex_sel = 0b001;  // ex1_pipedown asserted
        tick(dut);                           // EX1->EX2
        dut.dp_vfalu_ex1_pipex_sel = 0;
        tick(dut);  // EX2->EX3

        if (!expect(dut.fspu_forward_r_vld == 1, std::string(sc.name) + " forward valid missing")) return 1;
        if (!expect(dut.fspu_forward_result == exp.result,
                    std::string(sc.name) + " forward result mismatch got 0x" + [&]() {
                        std::ostringstream oss;
                        oss << std::hex << dut.fspu_forward_result << " expected 0x" << exp.result;
                        return oss.str();
                    }())) {
            return 1;
        }
        if (!expect(dut.fspu_mfvr_data == exp.mfvr,
                    std::string(sc.name) + " mfvr mismatch got 0x" + [&]() {
                        std::ostringstream oss;
                        oss << std::hex << dut.fspu_mfvr_data << " expected 0x" << exp.mfvr;
                        return oss.str();
                    }())) {
            return 1;
        }

        tick(dut);  // clear valids
    }

    auto drive_no_check = [&](uint32_t func, uint64_t s0, uint64_t s1, uint64_t mtvr) {
        dut.dp_vfalu_ex1_pipex_func = func;
        dut.dp_vfalu_ex1_pipex_srcf0 = s0;
        dut.dp_vfalu_ex1_pipex_srcf1 = s1;
        dut.dp_vfalu_ex1_pipex_mtvr_src0 = mtvr;
        dut.dp_vfalu_ex1_pipex_sel = 0b001;
        tick(dut);
        dut.dp_vfalu_ex1_pipex_sel = 0;
        tick(dut);
        tick(dut);
    };

    // Sweep classification and sign ops across representative patterns.
    std::vector<uint64_t> double_vals = {0x0000000000000000ULL, 0x8000000000000000ULL, 0x0000000000000001ULL, 0x8000000000000001ULL,
                                         0x3FF0000000000000ULL, 0xBFF0000000000000ULL, 0x7FF0000000000000ULL, 0xFFF0000000000000ULL,
                                         0x7FF8000000000000ULL, 0x7FF0000000000001ULL};
    std::vector<uint32_t> single_vals = {0x00000000u, 0x80000000u, 0x00000001u, 0x80000001u, 0x3F800000u, 0xBF800000u, 0x7F800000u,
                                         0xFF800000u, 0x7FC00000u, 0x7F800001u};
    std::vector<uint16_t> half_vals = {0x0000u, 0x8000u, 0x0001u, 0x8001u, 0x3C00u, 0xBC00u, 0x7C00u, 0xFC00u, 0x7E00u, 0x7D00u};

    for (auto v : double_vals) {
        drive_no_check(build_func(true, false, true, false, false, false, false, false), v, 0, 0);
    }
    for (auto v : single_vals) {
        drive_no_check(build_func(false, true, true, false, false, false, false, false), pack_single_val(v), 0, 0);
    }
    for (auto v : half_vals) {
        drive_no_check(build_func(false, false, true, false, false, false, false, false), pack_half_val(v), 0, 0);
    }

    auto sign_pairs = std::vector<std::pair<uint64_t, uint64_t>>{
        {0x3FF0000000000000ULL, 0xBFF0000000000000ULL}, {0xBFF0000000000000ULL, 0x3FF0000000000000ULL}};
    for (auto [a, b] : sign_pairs) {
        drive_no_check(build_func(true, false, false, false, false, true, false, false), a, b, 0);
        drive_no_check(build_func(true, false, false, false, false, false, true, false), a, b, 0);
        drive_no_check(build_func(true, false, false, false, false, false, false, true), a, b, 0);
    }
    auto sign_pairs_single = std::vector<std::pair<uint32_t, uint32_t>>{{0x3F800000u, 0xBF800000u}, {0xBF800000u, 0x3F800000u}};
    for (auto [a, b] : sign_pairs_single) {
        drive_no_check(build_func(false, true, false, false, false, true, false, false), pack_single_val(a), pack_single_val(b), 0);
        drive_no_check(build_func(false, true, false, false, false, false, true, false), pack_single_val(a), pack_single_val(b), 0);
        drive_no_check(build_func(false, true, false, false, false, false, false, true), pack_single_val(a), pack_single_val(b), 0);
    }
    auto sign_pairs_half = std::vector<std::pair<uint16_t, uint16_t>>{{0x3C00u, 0xBC00u}, {0xBC00u, 0x3C00u}};
    for (auto [a, b] : sign_pairs_half) {
        uint64_t pa = pack_half_val(a);
        uint64_t pb = pack_half_val(b);
        drive_no_check(build_func(false, false, false, false, false, true, false, false), pa, pb, 0);
        drive_no_check(build_func(false, false, false, false, false, false, true, false), pa, pb, 0);
        drive_no_check(build_func(false, false, false, false, false, false, false, true), pa, pb, 0);
    }

    // More mvfx/mvxf variations.
    for (uint64_t mv : {0x0ULL, 0xFFFFFFFFFFFFFFFFULL, 0x12345678ABCDEF01ULL}) {
        drive_no_check(build_func(true, false, false, true, false, false, false, false), 0, 0, mv);
        drive_no_check(build_func(false, true, false, true, false, false, false, false), 0, 0, mv);
        drive_no_check(build_func(false, false, false, true, false, false, false, false), 0, 0, mv);
    }
    for (uint64_t src : {0x3FF8000000000000ULL, 0xBFF0000000000000ULL, 0x7FF8000000000001ULL}) {
        drive_no_check(build_func(true, false, false, false, true, false, false, false), src, 0, 0);
    }
    for (uint64_t src : {0x3F800000ULL, 0xBF800000ULL, 0x7F800001ULL}) {
        drive_no_check(build_func(false, true, false, false, true, false, false, false), pack_single_val(static_cast<uint32_t>(src)), 0, 0);
    }
    for (uint64_t src : {pack_half_val(0x3C00u), pack_half_val(0xBC00u), pack_half_val(0x7DFFu)}) {
        drive_no_check(build_func(false, false, false, false, true, false, false, false), src, 0, 0);
    }

    // Toggle clocks to hit gating.
    dut.cp0_yy_clk_en = 0;
    tick(dut);
    dut.cp0_yy_clk_en = 1;
    tick(dut);
    dut.cp0_vfpu_icg_en = 0;
    tick(dut);
    dut.cp0_vfpu_icg_en = 1;

    // Randomized toggles for coverage.
    uint64_t lcg = 13;
    auto next = [&]() {
        lcg = lcg * 1103515245ULL + 12345ULL;
        return lcg;
    };
    for (int i = 0; i < 20000; ++i) {
        dut.dp_vfalu_ex1_pipex_func = static_cast<uint32_t>(next());
        dut.dp_vfalu_ex1_pipex_srcf0 = next();
        dut.dp_vfalu_ex1_pipex_srcf1 = next();
        dut.dp_vfalu_ex1_pipex_mtvr_src0 = next();
        dut.dp_vfalu_ex1_pipex_sel = 0b001;
        tick(dut);
        dut.dp_vfalu_ex1_pipex_sel = 0;
        tick(dut);
        tick(dut);
    }

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case030/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
