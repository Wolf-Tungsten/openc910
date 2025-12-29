#include <verilated.h>
#include <verilated_cov.h>

#include "Vct_fspu_dp.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

void tick(Vct_fspu_dp& dut) {
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

uint64_t pack_single(uint32_t low, uint32_t upper = 0xFFFFFFFFu) {
    return (static_cast<uint64_t>(upper) << 32) | low;
}

uint64_t pack_half(uint16_t low, uint64_t upper = 0xFFFFFFFFFFFFULL) {
    return (upper << 16) | low;
}

uint64_t apply_fsgnj(uint64_t oper0, uint64_t oper1, int width, int sign_lsb) {
    const uint64_t mask = (width == 64) ? ~0ULL : ((1ULL << width) - 1);
    const uint64_t payload_mask = mask >> 1;
    const uint64_t op0 = oper0 & mask;
    const uint64_t op1 = oper1 & mask;
    const uint64_t sign0 = (op0 >> sign_lsb) & 1ULL;
    const uint64_t sign1 = (op1 >> sign_lsb) & 1ULL;
    const uint64_t payload = op0 & payload_mask;
    const uint64_t res_sign = sign1;
    return (res_sign << sign_lsb) | payload;
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
    uint64_t ex1_result;
    uint64_t mfvr_data;
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
        uint64_t res = 0;
        if (op_fmvfx) {
            res = chosen_mtvr;
        } else if (op_fsgnj) {
            res = apply_fsgnj(sc.src0, sc.src1, 64, 63);
        } else if (op_fsgnjn) {
            res = apply_fsgnjn(sc.src0, sc.src1, 64, 63);
        } else if (op_fsgnjx) {
            res = apply_fsgnjx(sc.src0, sc.src1, 64, 63);
        }
        e.ex1_result = res;
        const uint64_t class_val = make_fclass_double(sc.src0);
        const uint64_t mvxf_val = sc.src0;
        e.mfvr_data = op_class ? class_val : (op_fmvxf ? mvxf_val : 0);
    } else if (is_single) {
        const bool cnan0 = (sc.src0 >> 32) != 0xFFFFFFFFULL;
        const bool cnan1 = (sc.src1 >> 32) != 0xFFFFFFFFULL;
        const uint32_t op0 = cnan0 ? 0x7fc00000u : static_cast<uint32_t>(sc.src0);
        const uint32_t op1 = cnan1 ? 0x7fc00000u : static_cast<uint32_t>(sc.src1);
        uint64_t res = 0;
        if (op_fmvfx) {
            // check_nan is 0 in the datapath instance for singles, so mtvr is not canonicalized.
            const uint32_t mtvr_f = static_cast<uint32_t>(chosen_mtvr);
            res = (0xFFFFFFFFULL << 32) | mtvr_f;
        } else if (op_fsgnj) {
            const uint32_t val = apply_fsgnj(op0, op1, 32, 31);
            res = (0xFFFFFFFFULL << 32) | val;
        } else if (op_fsgnjn) {
            const uint32_t val = apply_fsgnjn(op0, op1, 32, 31);
            res = (0xFFFFFFFFULL << 32) | val;
        } else if (op_fsgnjx) {
            const uint32_t val = apply_fsgnjx(op0, op1, 32, 31);
            res = (0xFFFFFFFFULL << 32) | val;
        }
        e.ex1_result = res;
        const uint64_t class_val = static_cast<uint64_t>(fclass_single_bits(op0));
        const uint64_t mvxf_val = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(sc.src0 & 0xFFFFFFFFu)));
        e.mfvr_data = op_class ? class_val : (op_fmvxf ? mvxf_val : 0);
    } else {  // half
        const bool cnan0 = (sc.src0 >> 16) != 0xFFFFFFFFFFFFULL;
        const bool cnan1 = (sc.src1 >> 16) != 0xFFFFFFFFFFFFULL;
        const uint16_t op0 = cnan0 ? static_cast<uint16_t>(0x7e00) : static_cast<uint16_t>(sc.src0 & 0xFFFF);
        const uint16_t op1 = cnan1 ? static_cast<uint16_t>(0x7e00) : static_cast<uint16_t>(sc.src1 & 0xFFFF);
        uint64_t res = 0;
        if (op_fmvfx) {
            const uint16_t mtvr_f = static_cast<uint16_t>(chosen_mtvr & 0xFFFF);
            res = (0xFFFFFFFFFFFFULL << 16) | mtvr_f;
        } else if (op_fsgnj) {
            const uint16_t val = static_cast<uint16_t>(apply_fsgnj(op0, op1, 16, 15));
            res = (0xFFFFFFFFFFFFULL << 16) | val;
        } else if (op_fsgnjn) {
            const uint16_t val = static_cast<uint16_t>(apply_fsgnjn(op0, op1, 16, 15));
            res = (0xFFFFFFFFFFFFULL << 16) | val;
        } else if (op_fsgnjx) {
            const uint16_t val = static_cast<uint16_t>(apply_fsgnjx(op0, op1, 16, 15));
            res = (0xFFFFFFFFFFFFULL << 16) | val;
        }
        e.ex1_result = res;
        const uint64_t class_val = static_cast<uint64_t>(fclass_half_bits(op0));
        const uint64_t mvxf_val = static_cast<uint64_t>(static_cast<int64_t>(static_cast<int16_t>(sc.src0 & 0xFFFF)));
        e.mfvr_data = op_class ? class_val : (op_fmvxf ? mvxf_val : 0);
    }
    return e;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vct_fspu_dp dut;

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
    dut.ex1_pipedown = 0;
    dut.ex2_pipedown = 0;
    dut.ex3_pipedown = 0;
    tick(dut);
    dut.cpurst_b = 1;
    dut.pad_yy_icg_scan_en = 1;
    tick(dut);

    std::vector<Scenario> scenarios = {
        {"double_fsgnj", build_func(true, false, false, false, false, true, false, false), 0x0123456789ABCDEFULL, 0xFEDCBA9876543210ULL, 0},
        {"single_fmvfx", build_func(false, true, false, true, false, false, false, false), 0x7F800001ULL, 0, 0x11223344ULL},
        {"half_class", build_func(false, false, true, false, false, false, false, false), 0xFFFF7C00ULL, 0, 0},
        {"single_fmvxf", build_func(false, true, false, false, true, false, false, false), 0x8000FFFFULL, 0, 0},
        {"double_class", build_func(true, false, true, false, false, false, false, false), 0x7FF8000000000000ULL, 0, 0},
    };

    for (const auto& sc : scenarios) {
        const Expected exp = compute_expected(sc);

        dut.dp_vfalu_ex1_pipex_func = sc.func;
        dut.dp_vfalu_ex1_pipex_srcf0 = sc.src0;
        dut.dp_vfalu_ex1_pipex_srcf1 = sc.src1;
        dut.dp_vfalu_ex1_pipex_mtvr_src0 = sc.mtvr;

        dut.ex1_pipedown = 1;
        dut.ex2_pipedown = 0;
        dut.ex3_pipedown = 0;
        tick(dut);

        dut.ex1_pipedown = 0;
        dut.ex2_pipedown = 1;
        dut.ex3_pipedown = 0;
        tick(dut);

        dut.ex2_pipedown = 0;
        dut.ex3_pipedown = 1;
        tick(dut);

        if (!expect(dut.fspu_forward_r_vld == 1, std::string(sc.name) + " forward valid missing")) return 1;
        if (!expect(dut.fspu_forward_result == exp.ex1_result, std::string(sc.name) + " forward result mismatch")) return 1;
        if (!expect(dut.fspu_mfvr_data == exp.mfvr_data, std::string(sc.name) + " mfvr data mismatch")) return 1;
    }

    auto run_op = [&](const Scenario& sc, bool check_outputs) {
        const Expected exp = compute_expected(sc);
        dut.dp_vfalu_ex1_pipex_func = sc.func;
        dut.dp_vfalu_ex1_pipex_srcf0 = sc.src0;
        dut.dp_vfalu_ex1_pipex_srcf1 = sc.src1;
        dut.dp_vfalu_ex1_pipex_mtvr_src0 = sc.mtvr;
        dut.ex1_pipedown = 1;
        dut.ex2_pipedown = 0;
        dut.ex3_pipedown = 0;
        tick(dut);
        dut.ex1_pipedown = 0;
        dut.ex2_pipedown = 1;
        tick(dut);
        dut.ex2_pipedown = 0;
        dut.ex3_pipedown = 1;
        tick(dut);
        if (check_outputs) {
            if (!expect(dut.fspu_forward_r_vld == 1, std::string(sc.name) + " forward valid missing")) return false;
            if (!expect(dut.fspu_forward_result == exp.ex1_result, std::string(sc.name) + " forward result mismatch")) return false;
            if (!expect(dut.fspu_mfvr_data == exp.mfvr_data, std::string(sc.name) + " mfvr data mismatch")) return false;
        }
        return true;
    };

    // Cover single/half classification corners with canonicalization disabled (upper bits all 1).
    auto drive_class = [&](uint64_t raw, bool is_single) {
        dut.dp_vfalu_ex1_pipex_func = is_single ? build_func(false, true, false, false, false, false, false, false)
                                                : build_func(false, false, false, false, false, false, false, false);
        const uint64_t upper_mask = is_single ? 0xFFFFFFFF00000000ULL : 0xFFFFFFFFFFFF0000ULL;
        dut.dp_vfalu_ex1_pipex_srcf0 = upper_mask | raw;
        dut.dp_vfalu_ex1_pipex_srcf1 = ~dut.dp_vfalu_ex1_pipex_srcf0;
        dut.dp_vfalu_ex1_pipex_mtvr_src0 = dut.dp_vfalu_ex1_pipex_srcf1;
        dut.ex1_pipedown = 1;
        dut.ex2_pipedown = 0;
        dut.ex3_pipedown = 0;
        tick(dut);
        dut.ex1_pipedown = 0;
        dut.ex2_pipedown = 1;
        tick(dut);
        dut.ex2_pipedown = 0;
        dut.ex3_pipedown = 1;
        tick(dut);
    };
    std::array<uint64_t, 7> single_classes = {0x00000000ULL, 0x00000001ULL, 0x3F800000ULL, 0x40000000ULL,
                                              0x7F800000ULL, 0x7F800001ULL, 0x80000000ULL};
    for (auto v : single_classes) {
        drive_class(v, true);
    }
    std::array<uint64_t, 7> half_classes = {0x0000ULL, 0x0001ULL, 0x3C00ULL, 0x7C00ULL, 0x7C01ULL, 0x8000ULL, 0x83FFULL};
    for (auto v : half_classes) {
        drive_class(v, false);
    }

    struct ClassCase {
        bool is_double;
        bool is_single;
        uint64_t src;
    };
    auto run_class_check = [&](const ClassCase& cc) {
        const uint32_t func = build_func(cc.is_double, cc.is_single, true, false, false, false, false, false);
        const uint64_t upper_mask = cc.is_double ? 0 : (cc.is_single ? 0xFFFFFFFF00000000ULL : 0xFFFFFFFFFFFF0000ULL);
        const uint64_t src = upper_mask | cc.src;
        const uint64_t expected =
            cc.is_double   ? make_fclass_double(src)
            : cc.is_single ? static_cast<uint64_t>(fclass_single_bits(static_cast<uint32_t>(src)))
                           : static_cast<uint64_t>(fclass_half_bits(static_cast<uint16_t>(src)));
        dut.dp_vfalu_ex1_pipex_func = func;
        dut.dp_vfalu_ex1_pipex_srcf0 = src;
        dut.dp_vfalu_ex1_pipex_srcf1 = ~src;
        dut.dp_vfalu_ex1_pipex_mtvr_src0 = src ^ 0xF0F0F0F0F0F0F0F0ULL;
        dut.ex1_pipedown = 1;
        dut.ex2_pipedown = 0;
        dut.ex3_pipedown = 0;
        tick(dut);
        dut.ex1_pipedown = 0;
        dut.ex2_pipedown = 1;
        tick(dut);
        dut.ex2_pipedown = 0;
        dut.ex3_pipedown = 1;
        tick(dut);
        expect(dut.fspu_mfvr_data == expected, "class check mfvr mismatch");
    };
    std::vector<ClassCase> class_cases = {
        {true, false, 0x8000000000000001ULL},  // double neg denorm
        {true, false, 0x7FF0000000000000ULL},  // double pos inf
        {true, false, 0x7FF0000000000001ULL},  // double snan
        {false, true, 0x80000001ULL},          // single neg dn
        {false, true, 0xFF800000ULL},          // single neg inf
        {false, true, 0x7FC00001ULL},          // single qnan
        {false, false, 0x8001ULL},             // half neg dn
        {false, false, 0xFC00ULL},             // half neg inf
        {false, false, 0x7E01ULL},             // half snan
    };
    for (const auto& cc : class_cases) {
        run_class_check(cc);
    }

    // Toggle clock enables to cover gating inputs.
    dut.cp0_yy_clk_en = 0;
    dut.cp0_vfpu_icg_en = 0;
    tick(dut);
    dut.cp0_yy_clk_en = 1;
    dut.cp0_vfpu_icg_en = 1;

    // Randomized stress to toggle more datapath logic.
    uint64_t lcg = 5;
    auto next = [&]() {
        lcg = lcg * 1664525ULL + 1013904223ULL;
        return lcg;
    };
    for (int i = 0; i < 512; ++i) {
        Scenario sc{
            "random",
            build_func((i & 1) != 0, (i & 2) != 0, (i & 4) != 0, (i & 8) != 0, (i & 16) != 0, (i & 32) != 0, (i & 64) != 0, ((i << 1) & 128) != 0),
            next(),
            next(),
            next(),
        };
        Expected exp = compute_expected(sc);
        dut.dp_vfalu_ex1_pipex_func = sc.func;
        dut.dp_vfalu_ex1_pipex_srcf0 = sc.src0;
        dut.dp_vfalu_ex1_pipex_srcf1 = sc.src1;
        dut.dp_vfalu_ex1_pipex_mtvr_src0 = sc.mtvr;
        dut.ex1_pipedown = 1;
        dut.ex2_pipedown = 0;
        dut.ex3_pipedown = 0;
        tick(dut);
        dut.ex1_pipedown = 0;
        dut.ex2_pipedown = 1;
        dut.ex3_pipedown = 0;
        tick(dut);
        dut.ex2_pipedown = 0;
        dut.ex3_pipedown = 1;
        tick(dut);
    }

    // Classification-focused sweeps with op_class asserted to toggle result_fclass buses in each precision.
    auto class_sweep = [&](bool is_double, bool is_single, const std::vector<uint64_t>& vals) {
        const uint32_t func = build_func(is_double, is_single, true, false, false, false, false, false);
        for (auto v : vals) {
            dut.dp_vfalu_ex1_pipex_func = func;
            dut.dp_vfalu_ex1_pipex_srcf0 = v;
            dut.dp_vfalu_ex1_pipex_srcf1 = ~v;
            dut.dp_vfalu_ex1_pipex_mtvr_src0 = v ^ 0x5555AAAA5555AAAAULL;
            dut.ex1_pipedown = 1;
            dut.ex2_pipedown = 0;
            dut.ex3_pipedown = 0;
            tick(dut);
            dut.ex1_pipedown = 0;
            dut.ex2_pipedown = 1;
            tick(dut);
            dut.ex2_pipedown = 0;
            dut.ex3_pipedown = 1;
            tick(dut);
        }
    };
    class_sweep(true, false,
                {0x0000000000000000ULL, 0x8000000000000000ULL, 0x0000000000000001ULL, 0x8000000000000001ULL,
                 0x7FF0000000000000ULL, 0xFFF0000000000000ULL, 0x7FF8000000000001ULL, 0x3FF0000000000000ULL,
                 0xBFF0000000000000ULL});
    class_sweep(false, true,
                {0x00000001ULL, 0x80000001ULL, 0x7F800000ULL, 0xFF800000ULL, 0x7FC00001ULL, 0x3F800000ULL,
                 0xBF800000ULL});
    class_sweep(false, false,
                {0x0001ULL, 0x8001ULL, 0x7C00ULL, 0xFC00ULL, 0x7C01ULL, 0x3C00ULL, 0xBC00ULL});

    std::vector<Scenario> half_ops = {
        {"half_fsgnj_raw", build_func(false, false, false, false, false, true, false, false), pack_half(0x3C01), pack_half(0xBC00), 0},
        {"half_fsgnjn_raw", build_func(false, false, false, false, false, false, true, false), pack_half(0x83FF), pack_half(0x0400), 0},
        {"half_fsgnjx_cnan", build_func(false, false, false, false, false, false, false, true), pack_half(0x7C01, 0x1234), pack_half(0x8000, 0), 0},
        {"half_fmvxf_neg", build_func(false, false, false, false, true, false, false, false), pack_half(0x8001), 0, 0},
        {"half_fmvfx_alt", build_func(false, false, false, true, false, false, false, false), pack_half(0x7BFF, 0), pack_half(0x3C00), pack_half(0xAA55, 0x00FF)},
    };
    for (const auto& sc : half_ops) {
        if (!run_op(sc, true)) return 1;
    }

    std::vector<Scenario> single_ops = {
        {"single_fsgnj_full", build_func(false, true, false, false, false, true, false, false), pack_single(0x3F000000), pack_single(0xBF800000), 0},
        {"single_fsgnjn_flip", build_func(false, true, false, false, false, false, true, false), pack_single(0xBF7FFFFF, 0), pack_single(0x3F800000, 0x0FFF), 0},
        {"single_fsgnjx_mix", build_func(false, true, false, false, false, false, false, true), pack_single(0x7FC00001, 0xAAAA5555), pack_single(0x00000001, 0xFFFFFFFF), 0},
        {"single_fmvxf_span", build_func(false, true, false, false, true, false, false, false), pack_single(0x80000001), 0, 0},
        {"single_fmvfx_mtvr", build_func(false, true, false, true, false, false, false, false), pack_single(0x00000001, 0), pack_single(0x7F800000, 0xFFFFFFFF), pack_single(0x11223344, 0x55667788)},
    };
    for (const auto& sc : single_ops) {
        if (!run_op(sc, true)) return 1;
    }

    std::vector<Scenario> mux_flips = {
        {"double_fmvxf_mux", build_func(true, false, false, false, true, false, false, false), 0xFFF0000000000001ULL, 0, 0},
        {"single_class_mux", build_func(false, true, true, false, false, false, false, false), pack_single(0xFF800000), 0, 0},
        {"half_fsgnj_mux", build_func(false, false, false, false, false, true, false, false), pack_half(0x83FF, 0x00AA00AA00AAULL), pack_half(0x3C00, 0xFFFFFFFFFFFFULL), 0},
        {"double_fmvfx_mux", build_func(true, false, false, true, false, false, false, false), 0, 0, 0xDEADBEEFCAFEBABEULL},
    };
    for (int rep = 0; rep < 2; ++rep) {
        for (const auto& sc : mux_flips) {
            if (!run_op(sc, false)) return 1;
        }
    }

    // Toggle func bus bits that are otherwise unused in the datapath decode.
    std::array<int, 12> func_bits = {3, 4, 7, 8, 9, 10, 11, 12, 13, 14, 17, 19};
    for (auto bit : func_bits) {
        Scenario sc{"func_toggle", static_cast<uint32_t>(1u << bit), next(), ~next(), next()};
        if (!run_op(sc, false)) return 1;
    }

    // Sweep FMVFX paths with varying payloads to toggle result buses in each precision.
    std::vector<uint16_t> half_mtvr_vals = {0x0000, 0xFFFF, 0xAAAA, 0x5555, 0x00FF, 0xFF00};
    for (auto v : half_mtvr_vals) {
        Scenario sc{"half_fmvfx_sweep", build_func(false, false, false, true, false, false, false, false), pack_half(0x3C00, 0xFFFFFFFFFFFFULL), pack_half(~static_cast<uint64_t>(v), 0xFFFFFFFFFFFFULL), pack_half(v, 0)};
        if (!run_op(sc, true)) return 1;
    }
    std::vector<uint32_t> single_mtvr_vals = {0x00000000u, 0xFFFFFFFFu, 0xAAAAAAAAu, 0x55555555u, 0x80000001u, 0x7F800000u};
    for (auto v : single_mtvr_vals) {
        Scenario sc{"single_fmvfx_sweep", build_func(false, true, false, true, false, false, false, false), pack_single(0x3F800000, 0xFFFFFFFFu), pack_single(~v, 0xFFFFFFFFu), pack_single(v, 0)};
        if (!run_op(sc, true)) return 1;
    }
    std::vector<uint64_t> double_mtvr_vals = {0x0ULL,
                                              0xFFFFFFFFFFFFFFFFULL,
                                              0xAAAAAAAAAAAAAAAAULL,
                                              0x5555555555555555ULL,
                                              0x8000000000000001ULL,
                                              0x7FF0000000000000ULL};
    for (auto v : double_mtvr_vals) {
        Scenario sc{"double_fmvfx_sweep", build_func(true, false, false, true, false, false, false, false), 0x123456789ABCDEF0ULL, ~v, v};
        if (!run_op(sc, true)) return 1;
    }

    // Explicit negative-normal classifications with upper bits set to avoid canonicalization collapsing to QNaN.
    Scenario half_neg_nm_class{ "half_neg_nm_class", build_func(false, false, true, false, false, false, false, false), pack_half(0xBC00, 0xFFFFFFFFFFFFULL), 0, 0};
    if (!run_op(half_neg_nm_class, true)) return 1;
    Scenario single_neg_nm_class{ "single_neg_nm_class", build_func(false, true, true, false, false, false, false, false), pack_single(0xBF800000, 0xFFFFFFFFu), 0, 0};
    if (!run_op(single_neg_nm_class, true)) return 1;
    Scenario single_class_reset{ "single_class_reset", build_func(false, true, true, false, false, false, false, false), pack_single(0x00000000, 0xFFFFFFFFu), 0, 0};
    if (!run_op(single_class_reset, true)) return 1;
    Scenario half_class_reset{ "half_class_reset", build_func(false, false, true, false, false, false, false, false), pack_half(0x0000, 0xFFFFFFFFFFFFULL), 0, 0};
    if (!run_op(half_class_reset, true)) return 1;

    const char* cov_out = std::getenv("COV_OUT");
    if (!cov_out) {
        cov_out = "build/case025/coverage.dat";
    }
    VerilatedCov::write(cov_out);
    return 0;
}
